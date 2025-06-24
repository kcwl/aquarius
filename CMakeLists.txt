cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/triplet")

if(WIN32)
set(VCPKG_TARGET_TRIPET "x64-windows-win10")

project(aquarius)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(AQUARIUS_PROTOCOL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/3rd/aquarius_protocol/include)

if(WIN32)
set(VCPKG_C_FLAG "-D_WIN32_WINNT=0x0A00 $VCPKG_C_FLAG")
set(VCPKG_CPP_FLAG "-D_WIN32_WINNT=0x0A00 $VCPKG_CPP_FLAG")
endif()

if(CMAKE_COVERAGE)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeCoverage.cmake)
APPEND_COVERAGE_COMPILER_FLAGS()
endif()

add_subdirectory(deps)

add_subdirectory(test)

enable_testing()