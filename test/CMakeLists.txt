project(unittest)

file(GLOB TEST_SRCS "*.h")

file(GLOB TEST_MAIN "*.cpp")

include_directories(${AQUARIUS_INCLUDE})

include_directories(${AQUARIUS_PROTOCOL_INCLUDE})

add_executable(unittest ${TEST_SRCS} ${TEST_MAIN})

target_link_libraries(unittest PUBLIC boost openssl)


if(WIN32)
target_compile_options(unittest PUBLIC /bigobj)
target_compile_definitions(unittest PUBLIC -DWIN32_LEAN_AND_MEAN)
if(USE_WIN7_FOR_BOOST_LOG)
message(STATUS "Set Config USE_WIN7_FOR_BOOST_LOG")
target_compile_definitions(unittest PUBLIC -DUSE_WIN7_FOR_BOOST_LOG)
endif()
endif()

if(COVERAGE)
message(STATUS "gcov")
target_compile_options(unittest PUBLIC -fprofile-arcs -ftest-coverage)
target_link_libraries(unittest PUBLIC gcov)
endif()


add_test(
	NAME unittest COMMAND unittest
)

add_custom_target(coverage 
	COMMAND rm -rf test-aquarius
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch -c -d . -o "coverage.info" -b .
	COMMAND lcov --ignore-errors inconsistent,negative,mismatch -e coverage.info "*/include/aquarius/*" -o coverage-aquarius.info
	COMMAND genhtml --ignore-errors inconsistent,negative,mismatch coverage-aquarius.info -o test-aquarius
)

