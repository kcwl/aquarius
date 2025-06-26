cmake_minimum_required(VERSION 3.12)

if(WIN32)
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/triplet")
#set(VCPKG_DEFAULT_TRIPLET "x64-windows-win10")
set(VCPKG_TARGET_TRIPLET "x64-windows-win10")
#message(STATUS "set vcpkg default triplet: ${VCPKG_DEFAULT_TRIPLET}")
message(STATUS "set vcpkg target triplet: ${VCPKG_TARGET_TRIPLET}")
endif()

project(aquarius)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(AQUARIUS_PROTOCOL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/3rd/aquarius_protocol/include)

if(CMAKE_COVERAGE)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeCoverage.cmake)
APPEND_COVERAGE_COMPILER_FLAGS()
endif()

add_subdirectory(deps)

add_subdirectory(test)

enable_testing()