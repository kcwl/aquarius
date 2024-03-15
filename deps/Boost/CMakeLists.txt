find_package(Boost REQUIRED)

add_library(boost INTERFACE)

target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})

target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIR})

target_compile_definitions(boost INTERFACE -DBOOST_LOG_DYN_LINK)

target_compile_options(boost INTERFACE -DBOOST_LOG_DYN_LINK)

find_package(Python3 3.8)

if(Python3_FOUND)

if(NOT Python3_LIBRARIES)
set(Python3_LIBRARIES "python3")
endif()

target_link_libraries(boost INTERFACE ${Python3_LIBRARIES})
endif()
