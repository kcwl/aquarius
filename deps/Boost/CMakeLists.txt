add_library(boost INTERFACE)

if(WIN32)
cmake_policy(SET CMP0074 NEW)

file(GLOB BOOST_DYN_LINKS "${BOOST_LIBS}/*.lib")
file(GLOB BOOST_LINK_LIBS "${BOOST_LIBS}/*.dll")

target_link_libraries(boost INTERFACE ${BOOST_DYN_LINKS} ${BOOST_LINK_LIBS})

message(STATUS "boost lib:${BOOST_DYN_LINKS}")
message(STATUS "boost dll:${BOOST_LINK_LIBS}")

target_link_directories(boost INTERFACE ${BOOST_LIBS})

target_include_directories(boost INTERFACE ${BOOST_ROOT})

message(STATUS "Boost ROOT ${BOOST_ROOT}")

else()

target_compile_options(boost INTERFACE -DBOOST_ALL_DYN_LINK)

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
