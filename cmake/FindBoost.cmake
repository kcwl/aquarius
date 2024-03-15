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
)

if(NOT INCLUDE_DIR)
message(STATUS FATAL_ERROR "Boost ${BOOST_VERSION} not found.")
endif()

list(APPEND Boost_INCLUDE_DIRS ${INCLUDE_DIR})

foreach(lib ${AQUARIUS_NEED_BOOST})
	unset(LIBS CACHE)
	find_library(LIBS boost_${lib})

	if(NOT LIBS)
		message(STATUS "boost_${lib} not found")
	endif()

	list(APPEND Boost_LIBRARIES ${LIBS})
endforeach()

if(Boost_INCLUDE_DIRS AND Boost_LIBRARIES)
set(Boost_FOUND true)
set(Boost_FOUND_VERSION ${BOOST_VERSION})
message(STATUS "Find Boost(${BOOST_VERSION})")
endif()


