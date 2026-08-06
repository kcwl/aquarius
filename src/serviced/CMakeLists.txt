project(aquarius_serviced)

file(GLOB SRC_LIST *.cpp proto/*.cpp)

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(PROTO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../serviced/proto/)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${INCLUDES} ${PROTO_INCLUDES})

target_link_libraries(${PROJECT_NAME} PRIVATE boost openssl)
