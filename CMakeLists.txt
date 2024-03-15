cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(aquarius)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(CMAKE_COVERAGE)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeCoverage.cmake)
ADD_COVERAGE_COMPILE_FLAGS()
endif()

add_subdirectory(deps)

add_subdirectory(src)

add_subdirectory(test)

enable_testing()