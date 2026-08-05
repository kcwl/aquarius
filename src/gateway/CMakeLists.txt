project(aquarius_gateway)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_LIST *.cpp proto/*.cpp)

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../)

add_executable(${PROJECT_NAME} ${SRC_LIST})


message(STATUS "${INCLUDES}")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${INCLUDES})

target_link_libraries(${PROJECT_NAME} PRIVATE boost)
target_compile_definitions(${PROJECT_NAME} PUBLIC HTTP_GATEWAY)