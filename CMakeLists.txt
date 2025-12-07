cmake_minimum_required(VERSION 3.31)

project(aquarius)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

add_subdirectory(deps)

add_subdirectory(test)