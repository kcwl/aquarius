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

set_target_properties(unittest PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK")


add_test(
	NAME unittest COMMAND unittest
)

if(CMAKE_COVERAGE)
setup_target_for_coverage_gcovr_xml(
	${PROJECT_NAME}_cover NAME coverage
)
endif()
