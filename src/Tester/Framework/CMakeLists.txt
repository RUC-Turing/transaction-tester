project(transaction-tester CXX)

file(GLOB SRC "*.cc" "**/*.cc")
add_library(transaction-tester SHARED ${SRC})

target_link_libraries(transaction-tester transaction-tester-algorithm argument-parser pthread)
