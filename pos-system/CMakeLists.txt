cmake_minimum_required(VERSION 3.10)
project(pos_system)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR} include)

file(GLOB SRC_FILES "src/*.cpp")
add_executable(branch_server ${SRC_FILES})
target_link_libraries(branch_server OpenSSL::SSL)

add_executable(tests tests/test.cpp src/sha256.cpp src/config.cpp)
target_link_libraries(tests OpenSSL::SSL)

find_package(CURL REQUIRED)
target_link_libraries(tests OpenSSL::SSL CURL::libcurl)