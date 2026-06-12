# Collect all unit-test source files recursively. Match files starting with 'ut_'.
file(GLOB_RECURSE UT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "ut_*.cpp")

# Path to test runner source that provides Boost.Test main
set(UNITTEST_RUNNER ${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp)
if(NOT EXISTS ${UNITTEST_RUNNER})
	message(FATAL_ERROR "Missing test runner: ${UNITTEST_RUNNER}")
endif()

# Include paths used by the tests
include_directories(${AQUARIUS_INCLUDE})
include_directories(${AQUARIUS_PROTOCOL_INCLUDE})

foreach(src IN LISTS UT_SOURCES)
	# skip the runner itself if matched
	if(src STREQUAL "unittest.cpp")
		continue()
	endif()

	# create a safe target name from the relative path: replace '/' with '_' and strip .cpp
	# string(REPLACE "/" "_" tgt_base ${src})
	string(REPLACE ".cpp" "" tgt_base ${src})

	string(FIND ${tgt_base} "/" pos REVERSE)

	math(EXPR pos "${pos} + 1" OUTPUT_FORMAT DECIMAL)

	string(SUBSTRING ${tgt_base} ${pos} -1 tgt_base)

	set(target_name ${tgt_base})

	add_executable(${target_name} ${src} ${UNITTEST_RUNNER})
	target_include_directories(${target_name} PRIVATE ${AQUARIUS_INCLUDE} ${AQUARIUS_PROTOCOL_INCLUDE})
	# target_link_libraries(${target_name} PRIVATE ${TEST_COMMON_LIBS})
	target_link_libraries(${target_name} PRIVATE boost)

	if(ENABLE_MYSQL)
		target_compile_definitions(${target_name} PUBLIC -DENABLE_MYSQL)
		target_link_libraries(${target_name} PRIVATE mysql)
	endif()

	if(ENABLE_SSL_TEST)
		target_compile_definitions(${target_name} PUBLIC -DENABLE_SSL_TEST)
		target_link_libraries(${target_name} PRIVATE openssl)
	endif()

	add_test(NAME ${target_name} COMMAND ${target_name})

	if(NOT WIN32)
		set_tests_properties(${target_name} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=/usr/local/gcc-16.1/lib64:/usr/local/lib:/usr/local/lib64:$ENV{LD_LIBRARY_PATH}")
	endif()
endforeach()

if(WIN32)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:unittest>)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:boost>)
#set_property(TEST unittest PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:mysql>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_custom_target(coverage 
	COMMAND rm -rf test-aquarius
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch,format,version -c -d . -o "coverage.info" -b .
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch,format,version -r coverage.info "*/boost/*" -o coverage-no-boost.info
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch,format,version -e coverage-no-boost.info "*/include/aquarius/*" -o coverage-aquarius.info
	COMMAND genhtml --ignore-errors inconsistent,negative,mismatch,format,version coverage-aquarius.info -o test-aquarius
)
