set(BOOST_VERSION 1.75.0)

find_package(Boost ${BOOST_VERSION} QUIET NO_MODULE)
if(Boost_FOUND)
message("Boost ${BOOST_VERSION} found.")
endif()

unset(INCLUDE_DIR CACHE)
find_path(INCLUDE_DIR
	NAMES boost/version.hpp
)

if(NOT INCLUDE_DIR)
message(STATUS FATAL_ERROR "Boost ${BOOST_VERSION} not found.")
endif()

list(APPEND Boost_INCLUDE_DIRS ${INCLUDE_DIR})

unset(LIBS CACHE)


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPUX "64")
endif()

file(GLOB BOOST_LIB_PATH "libboost*.so" "/usr/lib${CPUX}")

file(GLOB LIBS "${BOOST_LIB_PATH}/libboost*.so")

if(LIBS)
list(APPEND Boost_LIBRARIES ${LIBS})
endif()

if(Boost_INCLUDE_DIRS AND Boost_LIBRARIES)
set(Boost_FOUND true)
set(Boost_FOUND_VERSION ${BOOST_VERSION})
message(STATUS "Find Boost(${BOOST_VERSION})")
endif()


