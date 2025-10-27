project(lazytool)

file(GLOB SRC_LIST *.cpp)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

target_sources(${PROJECT_NAME} PUBLIC ${SRC_LIST})