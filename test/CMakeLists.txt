cmake_minimum_required(VERSION 3.10)
project(unittest LANGUAGES CXX)

# Find Boost components and OpenSSL for proper imported targets
find_package(Boost REQUIRED COMPONENTS log log_setup thread filesystem system date_time regex url json charconv unit_test_framework)
find_package(OpenSSL REQUIRED)

# Collect all unit-test source files recursively. Match files starting with 'ut_'.
file(GLOB_RECURSE UT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "ut_*.cpp")

# Also include any top-level test files that may not start with 'ut_' but are tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp")
	list(APPEND UT_SOURCES "unittest.cpp")
endif()

# Include paths used by the tests
include_directories(${AQUARIUS_INCLUDE})
include_directories(${AQUARIUS_PROTOCOL_INCLUDE})

# Build a single unittest executable that includes all UT_SOURCES and the
# header-only Boost.Test `unittest.cpp` which provides the test runner main.
add_executable(unittest ${UT_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp)

target_include_directories(unittest PRIVATE ${AQUARIUS_INCLUDE} ${AQUARIUS_PROTOCOL_INCLUDE})

target_link_libraries(unittest PRIVATE
	Boost::log_setup
	Boost::log
	Boost::thread
	Boost::filesystem
	Boost::system
	Boost::date_time
	Boost::regex
	Boost::json
	Boost::charconv
	Boost::url
	Boost::unit_test_framework
	OpenSSL::SSL
	OpenSSL::Crypto
	pthread
)

if(ENABLE_MYSQL)
	target_compile_definitions(unittest PUBLIC -DENABLE_MYSQL)
	target_link_libraries(unittest PRIVATE mysql)
endif()

if(ENABLE_SSL_TEST)
	target_compile_definitions(unittest PUBLIC -DENABLE_SSL_TEST)
endif()

add_test(NAME unittest COMMAND unittest)

if(NOT WIN32)
	set_tests_properties(unittest PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=/usr/local/gcc-16.1/lib64:/usr/local/lib:/usr/local/lib64:$ENV{LD_LIBRARY_PATH}")
endif()

if(WIN32)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:unittest>)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:boost>)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:mysql>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_custom_target(coverage 
	COMMAND rm -rf test-aquarius
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch -c -d . -o "coverage.info" -b .
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch -r coverage.info "*/boost/*" -o coverage-no-boost.info
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch -e coverage-no-boost.info "*/include/aquarius/*" -o coverage-aquarius.info
	COMMAND genhtml --ignore-errors inconsistent,negative,mismatch coverage-aquarius.info -o test-aquarius
)
