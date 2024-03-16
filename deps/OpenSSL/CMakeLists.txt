find_package(OpenSSL)

if(NOT OpenSSL_FOUND)
message(STATUS FATAL_ERROR "OpenSSL not found.")
endif()

add_library(openssl INTERFACE)

target_link_libraries(openssl INTERFACE ${OpenSSL_LIBRARIES})

target_include_directories(openssl INTERFACE ${OpenSSL_INCLUDE_DIR})

message(STATUS "Found ${OpenSSL_FOUND_VERSION}")