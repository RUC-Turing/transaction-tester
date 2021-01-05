project(transaction-tester CXX)

file(GLOB SRC "*.cc" "**/*.cc" "**/*.cpp")
add_executable(interactive-test ${SRC})

target_link_libraries(interactive-test transaction-tester transaction-tester-algorithm argument-parser)
