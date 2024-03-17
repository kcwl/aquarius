find_package(OpenSSL QUIET NO_MODULE)
if(OpenSSL_FOUND)
message("OpenSSL ${OpenSSL_FOUND_VERSION} found.")
endif()


if(WIN32)
set(OPENSSL_ROOT $ENV{OPENSSL_ROOT})

if(NOT OPENSSL_ROOT)
message(FATAL_ERROR "OpenSSL not found. maybe need to set OPENSSL_ROOT.")
endif()

set(OpenSSL_INCLUDE_DIRS ${OPENSSL_ROOT}/include)
file(GLOB OpenSSL_LIBRARIES "*.lib" ${OPENSSL_ROOT}/lib/*.lib)

else()

unset(INCLUDE_DIR CACHE)

file (GLOB INCLUDE_DIR "openssl/*" /usr/include)

if(NOT INCLUDE_DIR)
message(FATAL_ERROR "OpenSSL not found.")
endif()

list(APPEND OpenSSL_INCLUDE_DIRS ${INCLUDE_DIR})

unset(LIBS CACHE)

find_library(OpenSSL_CRYPTO crypto)

find_library(OpenSSL_SSL ssl)

list(APPEND OpenSSL_LIBRARIES ${OpenSSL_CRYPTO} ${OpenSSL_SSL})
endif()

set(OPENSSL_COMMAND "openssl")
set(COMMAND_ARGS "version")
execute_process(COMMAND ${OPENSSL_COMMAND} ${COMMAND_ARGS} OUTPUT_VARIABLE OPENSSL_VERSION)

string(STRIP "${OPENSSL_VERSION}" OPENSSL_VERSION)

if(OpenSSL_INCLUDE_DIRS AND OpenSSL_LIBRARIES)
set(OpenSSL_FOUND true)
set(OpenSSL_FOUND_VERSION ${OPENSSL_VERSION})
endif()


