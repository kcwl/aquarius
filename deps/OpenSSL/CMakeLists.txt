if(WIN32)
cmake_policy(SET CMP0074 NEW)
endif()

if (NOT AQUARIUS_ENABLE_SSL)
return()
endif()

find_package(OpenSSL CONFIG)

if(NOT OpenSSL_FOUND)
message(STATUS FATAL_ERROR "OpenSSL not found.")
endif()

add_library(openssl INTERFACE)

target_link_libraries(openssl INTERFACE ${OpenSSL_LIBRARIES})

target_include_directories(openssl INTERFACE ${OpenSSL_INCLUDE_DIRS})

message(STATUS "Found ${OpenSSL_FOUND_VERSION}")