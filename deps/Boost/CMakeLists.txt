add_library(boost INTERFACE)
target_compile_options(boost INTERFACE -DBOOST_ALL_DYN_LINK)

if(WIN32)
cmake_policy(SET CMP0074 NEW)

file(GLOB BOOST_LINK_LIBS "${BOOST_LIBS}/*.dll")

target_link_libraries(boost INTERFACE ${BOOST_LINK_LIBS})

target_link_directories(boost INTERFACE ${BOOST_ROOT})

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

target_link_libraries(boost INTERFACE Boost::unit_test_framework Boost::log Boost::log_setup Boost::system Boost::program_options Boost::json Boost::charconv)

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})
endif()
