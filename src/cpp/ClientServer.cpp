#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib> 
#include <string>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
namespace asio = boost::asio;
using boost::system::error_code;
using json = nlohmann::json;

std::string toHexString(const std::vector<char>& data) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    for (unsigned char c : data) {
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}

std::vector<char> prepareMessageForJavaUTF(const std::string& message) {
    // Java's DataInputStream.readUTF() expects the first two bytes to contain the length
    // of the string in UTF format, followed by the string itself.
    unsigned short len = htons(static_cast<unsigned short>(message.length())); // Convert length to network byte order

    std::vector<char> formattedMessage;
    formattedMessage.resize(2 + message.length()); // 2 bytes for the length, plus the message length

    std::memcpy(formattedMessage.data(), &len, 2); // Copy the length to the first 2 bytes
    std::memcpy(formattedMessage.data() + 2, message.c_str(), message.length()); // Copy the string data

    return formattedMessage;
}

std::string decompressGzip(const std::vector<char>& compressedData) {
    z_stream strm = {};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressedData.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));

    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        throw std::runtime_error("inflateInit2 failed");
    }

    int ret;
    char outbuffer[4096];
    std::string decompressedData;

    do {
        strm.avail_out = sizeof(outbuffer);
        strm.next_out = reinterpret_cast<Bytef*>(outbuffer);
        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  // state not clobbered
        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     // and fall through
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                throw std::runtime_error("inflate failed");
        }
        int have = sizeof(outbuffer) - strm.avail_out;
        decompressedData.append(outbuffer, have);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);

    return decompressedData;
}

int main() {
    const std::string server_ip = "127.0.0.1";
    const std::string server_port = "1234";
    std::cout << "Main" << std::endl;

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(server_ip, server_port);
    tcp::socket socket(io_context);
    boost::system::error_code ec;
    connect(socket, endpoints, ec);

    if (ec) {
        std::cerr << "Failed to connect to server: " << ec.message() << std::endl;
        return 1;
    }
    std::cout << "Connected to server." << std::endl;

    try {
        asio::streambuf buffer;
        std::istream input_stream(&buffer);
        std::string line;

        while (true) {
            // First, read the 2-byte length prefix (big-endian) for the UTF string
            size_t len = asio::read(socket, buffer, asio::transfer_exactly(2));
            std::vector<unsigned char> lengthBytes(2);
            input_stream.read(reinterpret_cast<char*>(lengthBytes.data()), 2);
            unsigned int length = (lengthBytes[0] << 8) | lengthBytes[1]; // Convert to unsigned int

            // Read the UTF string of the given length (dataType)
            std::string dataType;
            dataType.resize(length);
            len = asio::read(socket, buffer, asio::transfer_exactly(length));
            input_stream.read(&dataType[0], length);
            // If there's a null terminator in the string, remove it
            dataType.erase(std::remove(dataType.begin(), dataType.end(), '\0'), dataType.end());

            std::cout << "Data Type: " << dataType << std::endl;

            // Read the length of the JSON data
            len = asio::read(socket, buffer, asio::transfer_exactly(4));
            std::vector<unsigned char> jsonLengthBytes(4);
            input_stream.read(reinterpret_cast<char*>(jsonLengthBytes.data()), 4);
            unsigned int jsonLength = (jsonLengthBytes[0] << 24) |
                                    (jsonLengthBytes[1] << 16) |
                                    (jsonLengthBytes[2] << 8)  |
                                    jsonLengthBytes[3]; // Convert to unsigned int

            // Read the JSON data of the given length
            std::vector<char> jsonData(jsonLength);
            len = asio::read(socket, buffer, asio::transfer_exactly(jsonLength));
            input_stream.read(jsonData.data(), jsonLength);

            // Now, jsonData contains the compressed JSON data, decompress and parse it
            try {
                std::string decompressedJson = decompressGzip(jsonData);
                auto jsonParsed = json::parse(decompressedJson);
                std::cout << "JSON Data: " << jsonParsed.dump(4) << std::endl;
                // Based on the dataType, handle the parsed JSON appropriately
                // ...
            } catch (const std::exception& e) {
                std::cerr << "Error handling JSON data: " << e.what() << std::endl;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
