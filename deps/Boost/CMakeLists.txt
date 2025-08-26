if(WIN32)
cmake_policy(SET CMP0074 NEW)
endif()

set(AQUARIUS_NEED_BOOST 
	unit_test_framework
	log
	log_setup
	system
)

find_package(Boost 1.88 CONFIG REQUIRED COMPONENTS ${AQUARIUS_NEED_BOOST})

if(NOT Boost_FOUND)
message(FATAR_ERROR "Found Boost ${Boost_VERSION}")
return()
endif()

message(STATUS "Found Boost ${Boost_VERSION}")

add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE Boost::unit_test_framework Boost::log Boost::log_setup Boost::system)

target_compile_options(boost INTERFACE -DBOOST_ALL_DYN_LINK)

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})