project(aquarius_http_gateway)

file(GLOB SRC_LIST *.cpp proto/*.cpp)
file(GLOB PROTO_LIST ${CMAKE_CURRENT_SOURCE_DIR}/../serviced/proto/regist.virgo.cpp)

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(PROTO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../serviced/proto/)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${PROTO_LIST})

set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/../)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include ${INCLUDES} ${PROTO_INCLUDES})

target_link_libraries(${PROJECT_NAME} PRIVATE boost openssl)
