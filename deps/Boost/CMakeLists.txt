add_library(boost SHARED)
target_compile_options(boost PUBLIC -DBOOST_ALL_DYN_LINK)

if(WIN32)
cmake_policy(SET CMP0074 NEW)

file(GLOB BOOST_DYN_LINKS "${BOOST_LIBS}/*.lib")

target_link_libraries(boost PUBLIC ${BOOST_DYN_LINKS})

target_link_directories(boost PUBLIC ${BOOST_LIBS})

target_include_directories(boost PUBLIC ${BOOST_ROOT})

message(STATUS "Boost ROOT ${BOOST_ROOT}")

else()

set(AQUARIUS_NEED_BOOST 
	unit_test_framework
	log
	log_setup
	system
	json
	charconv
	program_options
)

find_package(Boost 1.88 CONFIG REQUIRED COMPONENTS ${AQUARIUS_NEED_BOOST})

if(NOT Boost_FOUND)
message(FATAR_ERROR "Found Boost ${Boost_VERSION}")
return()
endif()

message(STATUS "Found Boost ${Boost_VERSION}")

target_link_libraries(boost PUBLIC Boost::unit_test_framework Boost::log Boost::log_setup Boost::system Boost::program_options Boost::json Boost::charconv)

target_include_directories(boost PUBLIC ${Boost_INCLUDE_DIRS})
endif()
