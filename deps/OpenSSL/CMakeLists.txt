if(WIN32)
cmake_policy(SET CMP0074 NEW)
endif()

find_package(OpenSSL)

if(NOT OpenSSL_FOUND)
message(STATUS FATAL_ERROR "OpenSSL not found.")
return()
endif()

message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

add_library(openssl INTERFACE)

target_link_libraries(openssl INTERFACE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(openssl INTERFACE ${OPENSSL_INCLUDE_DIR})