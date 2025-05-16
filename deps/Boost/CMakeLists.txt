if(WIN32)
cmake_policy(SET CMP0074 NEW)
endif()

find_package(Boost CONFIG REQUIRED)

add_library(boost INTERFACE)

if(WIN32)
target_link_directories(boost INTERFACE ${Boost_LIBRARIES})
else()
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})
endif()

if(BOOST_SHARED_FLAG)
target_compile_definitions(boost INTERFACE -DBOOST_LOG_DYN_LINK)
endif()


target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})