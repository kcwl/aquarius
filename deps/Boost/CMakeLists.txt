find_package(Boost REQUIRED)

add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIR})

target_compile_definitions(boost INTERFACE BOOST_ALL_DYN_LINK BOOST_LOG_DYN_LINK)


find_package(Python3 3.8)

if(Python3_FOUND)
target_include_directories(boost INTERFACE ${Python3_INCLUDE_DIRS})
target_link_libraries(boost INTERFACE ${Python3_LIBRARIES})
endif()
