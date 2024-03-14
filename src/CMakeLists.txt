project(generator)

file(GLOB SRCS "*.cpp")

file(GLOB INCLUDES "*.h" "*.hpp")

add_executable(generator ${SRCS} ${INCLUDES})