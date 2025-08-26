cmake_minimum_required(VERSION 3.31)

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/triplet")

if(WIN32)
set(VCPKG_TARGET_TRIPLET "x64-windows-aquarius")
else()
set(VCPKG_TARGET_TRIPLET "x64-linux-aquarius")
endif()

message(STATUS "set vcpkg target triplet: ${VCPKG_TARGET_TRIPLET}")

project(aquarius)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(AQUARIUS_PROTOCOL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/3rd/virgo/include)

if(CMAKE_COVERAGE)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeCoverage.cmake)
APPEND_COVERAGE_COMPILER_FLAGS()
endif()

enable_testing()

add_subdirectory(deps)

add_subdirectory(test)

