project(unittest)

file(GLOB TEST_SRCS "*.h")

file(GLOB TEST_MAIN "*.cpp")

include_directories(${AQUARIUS_INCLUDE})

include_directories(${GTEST_INCLUDE})

add_executable(unittest ${TEST_SRCS} ${TEST_MAIN})

target_link_libraries(unittest PUBLIC boost openssl)

add_test(
	NAME unittest COMMAND unittest
)

if(CMAKE_COVERAGE)
setup_target_for_coverage_gcovr_xml(
	${PROJECT_NAME}_cover NAME coverage
)
endif()