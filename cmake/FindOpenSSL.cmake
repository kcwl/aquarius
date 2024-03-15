find_package(OpenSSL QUIET NO_MODULE)
if(OpenSSL_FOUND)
message("OpenSSL ${OpenSSL_FOUND_VERSION} found.")
endif()

unset(INCLUDE_DIR CACHE)

file (GLOB INCLUDE_DIR "openssl/*" /usr/include)

if(NOT INCLUDE_DIR)
message(STATUS FATAL_ERROR "OpenSSL Includes not found.")
endif()

list(APPEND OpenSSL_INCLUDE_DIRS ${INCLUDE_DIR})

unset(LIBS CACHE)

find_library(OpenSSL_CRYPTO crypto)

find_library(OpenSSL_SSL ssl)

list(APPEND OpenSSL_LIBRARIES ${OpenSSL_CRYPTO} ${OpenSSL_SSL})

if(OpenSSL_INCLUDE_DIRS AND OpenSSL_LIBRARIES)
set(OpenSSL_FOUND true)
message(STATUS "Find OpenSSL")
endif()


