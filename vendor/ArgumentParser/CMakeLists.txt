project(argument-parser CXX)

file(GLOB SRC "*.cc")

add_library(argument-parser SHARED ${SRC})
