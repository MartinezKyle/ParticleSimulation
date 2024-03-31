cmake_minimum_required(VERSION 3.29)

# Set your project name and specify the language (CXX for C++)
project(DistributedParticleExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system)
find_package(ZLIB REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/lib 
    $ENV{JAVA_HOME}/include
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

if(WIN32)
    include_directories($ENV{JAVA_HOME}/include/win32)
elseif(UNIX AND NOT APPLE)
    include_directories($ENV{JAVA_HOME}/include/linux)
elseif(APPLE)
    include_directories($ENV{JAVA_HOME}/include/darwin)
endif()

add_executable(ClientServer src/cpp/ClientServer.cpp)

target_link_libraries(ClientServer PRIVATE 
    Boost::system
    ZLIB::ZLIB
)
