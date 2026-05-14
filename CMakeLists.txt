cmake_minimum_required(VERSION 3.31)

project(aquarius)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AQUARIUS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

# Optional coverage build
option(COVERAGE "Enable coverage instrumentation" OFF)
if(COVERAGE)
	message(STATUS "Coverage instrumentation enabled")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_options(-g -O0 -fprofile-arcs -ftest-coverage)
		add_link_options(-fprofile-arcs -ftest-coverage)
	endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(deps)

add_subdirectory(test)