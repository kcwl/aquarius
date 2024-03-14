cmake_minimum_required(VERSION 3.2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(aquarius)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(GTEST_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include)

set(GTEST_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(WIN32)
set(LIBS "gtest.lib gtest_main.lib")
else()
set(LIBS "libgtest.a libgtest_main.a")
endif()

add_subdirectory(src)

add_subdirectory(test)

enable_testing()