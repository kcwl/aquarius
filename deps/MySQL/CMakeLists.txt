if(WIN32)
cmake_policy(SET CMP0074 NEW)
endif()

find_package(MySQL REQUIRED)

if(NOT MYSQL_VERSION)
message(FATAL_ERROR "MySQL not found.")
return()
endif()

message(STATUS "Found MySQL ${MYSQL_VERSION}")

add_library(mysql INTERFACE)

target_link_libraries(mysql PUBLIC ${MYSQL_LIB})

target_include_directories(mysql PUBLIC ${MYSQL_INCLUDE_DIR})