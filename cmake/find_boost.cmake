set(BOOST_VERSION 1.75)

find_package(Boost ${BOOST_VERSION} REQUIRED asio unit_test_framework)

if(NOT Boost_FOUND)
message("Boost ${BOOST_VERSION} is not found")
endif()

add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE ${Boost_Libraries})

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})