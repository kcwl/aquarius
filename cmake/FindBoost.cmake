set(BOOST_VERSION 1.75.0)

set(AQUARIUS_NEED_BOOST 
	unit_test_framework
	json
	log
	thread
	system
)

find_package(Boost ${BOOST_VERSION} QUIET NO_MODULE COMPONENTS ${AQUARIUS_NEED_BOOST})

if(Boost_FOUND)
message(STATUS "Boost ${BOOST_VERSION} found.")
return()
endif()

if(WIN32)
	set(BOOST_ROOT $ENV{BOOST_ROOT})
	if(NOT BOOST_ROOT)
		message(FATAL_ERROR "BOOST_ROOT not found, maybe need to set")
	endif()

	set(Boost_INCLUDE_DIRS ${BOOST_ROOT})
	set(Boost_LIBRARIES ${BOOST_ROOT}/stage/lib/)

else()
	unset(INCLUDE_DIR CACHE)
	find_path(INCLUDE_DIR
		NAMES boost/version.hpp
		HINTS ./
	)

	if(NOT INCLUDE_DIR)
	message(FATAL_ERROR "Boost not found.")
	endif()

	list(APPEND Boost_INCLUDE_DIRS ${INCLUDE_DIR})

	foreach(lib ${AQUARIUS_NEED_BOOST})
	unset(LIBS CACHE)
	find_library(LIBS boost_${lib})

	if(NOT LIBS)
		message(STATUS "boost_${lib} not found")
	endif()

	list(APPEND Boost_LIBRARIES ${LIBS})

	string(APPEND BOOST_COMPONENTS ${lib})
	string(APPEND BOOST_COMPONENTS " ")
	endforeach()
endif()

file(READ ${Boost_INCLUDE_DIRS}/boost/version.hpp BOOST_VERSION_HPP)

if(BOOST_VERSION_HPP)
string(REGEX MATCH "\"[0-9]_[0-9]+\"" BOOST_VERSION_INT ${BOOST_VERSION_HPP})
endif()

string(REPLACE "\"" "" BOOST_VERSION_INT ${BOOST_VERSION_INT})
string(REPLACE "_" "." BOOST_VERSION_INT ${BOOST_VERSION_INT})

if(Boost_INCLUDE_DIRS AND Boost_LIBRARIES)
set(Boost_FOUND true)
set(Boost_FOUND_VERSION ${BOOST_VERSION_INT})

if(BOOST_COMPONENTS)
set(BOOST_COMPONENTS " Components ${BOOST_COMPONENTS}")
endif()

message(STATUS "Find Boost(${Boost_FOUND_VERSION}) ${BOOST_COMPONENTS}")
endif()


