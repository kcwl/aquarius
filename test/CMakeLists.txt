project(unittest)

file(GLOB TEST_SRCS "*.h")

file(GLOB TEST_MAIN "*.cpp")

include_directories(${AQUARIUS_INCLUDE})

include_directories(${AQUARIUS_PROTOCOL_INCLUDE})

add_executable(unittest ${TEST_SRCS} ${TEST_MAIN})

target_link_libraries(unittest PUBLIC boost)

if (AQUARIUS_ENABLE_SSL)
target_link_libraries(unittest PUBLIC openssl)
endif()

target_compile_definitions(unittest PUBLIC -DBOOST_ASIO_HAS_INLINE_VARIABLES)

if(WIN32)
target_compile_definitions(unittest PUBLIC -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)
target_compile_options(unittest PUBLIC /bigobj )
endif()



add_test(
	NAME unittest COMMAND unittest
)

if(CMAKE_COVERAGE)
setup_target_for_coverage_gcovr_xml(
	${PROJECT_NAME}_cover NAME coverage
)
endif()
