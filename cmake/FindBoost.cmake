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
message("Boost ${BOOST_VERSION} found.")
return()
endif()

unset(INCLUDE_DIR CACHE)
find_path(INCLUDE_DIR
	NAMES boost/version.hpp
	HINTS ./
)

if(NOT INCLUDE_DIR)
message(STATUS "Boost not found.")
return()
endif()

list(APPEND Boost_INCLUDE_DIRS ${INCLUDE_DIR})

file(READ ${Boost_INCLUDE_DIRS}/boost/version.hpp BOOST_VERSION_HPP)

if(BOOST_VERSION_HPP)
string(REGEX MATCH "\"[0-9]_[0-9]+\"" BOOST_VERSION_INT ${BOOST_VERSION_HPP})
endif()


string(REPLACE "\"" "" BOOST_VERSION_INT ${BOOST_VERSION_INT})
string(REPLACE "_" "." BOOST_VERSION_INT ${BOOST_VERSION_INT})



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

if(Boost_INCLUDE_DIRS AND Boost_LIBRARIES)
set(Boost_FOUND true)
set(Boost_FOUND_VERSION ${BOOST_VERSION_INT})
message(STATUS "Find Boost(${Boost_FOUND_VERSION}) Components ${BOOST_COMPONENTS}")
endif()


