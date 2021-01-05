project(transaction-tester CXX)

file(GLOB SRC "*.cc" "**/*.cc")
add_executable(conflict-test ${SRC})

target_link_libraries(conflict-test transaction-tester transaction-tester-algorithm argument-parser)
