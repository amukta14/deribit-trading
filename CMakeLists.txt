cmake_minimum_required(VERSION 3.10)
project(GoQuant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Main executable
add_executable(goquant 
    src/main.cpp
)

# Link libraries
target_link_libraries(goquant
    PRIVATE
    goquant_core
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    Boost::system
)

# Include directories
target_include_directories(goquant
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
) 