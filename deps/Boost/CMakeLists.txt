find_package(Boost REQUIRED)

add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIR})

target_compile_definitions(boost INTERFACE -DBOOST_LOG_DYN_LINK)