cmake_minimum_required(VERSION 3.29)

# Set your project name and specify the language (CXX for C++)
project(DistributedParticleExplorer LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem)
find_package(ZLIB REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/lib  # This assumes you have a 'lib' directory in your source directory
    $ENV{JAVA_HOME}/include
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

# Include the correct directory based on the operating system
if(WIN32)
    include_directories($ENV{JAVA_HOME}/include/win32)
elseif(UNIX AND NOT APPLE)
    include_directories($ENV{JAVA_HOME}/include/linux)
elseif(APPLE)
    include_directories($ENV{JAVA_HOME}/include/darwin)
endif()

# Add the executable
add_executable(ClientServer src/cpp/ClientServer.cpp)

# Link libraries
target_link_libraries(ClientServer PRIVATE 
    ${Boost_LIBRARIES}
    ZLIB::ZLIB
    # If you have other libraries to link (like zlib or others), specify them here.
)
