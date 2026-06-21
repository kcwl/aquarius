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

# Optional AddressSanitizer build
option(ASAN "Enable AddressSanitizer instrumentation" OFF)
if(ASAN)
	message(STATUS "AddressSanitizer enabled")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-g -O1 -fno-omit-frame-pointer -fsanitize=address)
		add_link_options(-fsanitize=address)
	else()
		message(WARNING "ASAN set but compiler is not GNU/Clang; ASAN flags not applied")
	endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(deps)

add_subdirectory(test)