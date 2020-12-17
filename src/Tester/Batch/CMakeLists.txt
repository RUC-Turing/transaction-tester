project(transaction-tester CXX)

set(CMAKE_CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "*.cc" "**/*.cc")
add_executable(batch-test ${SRC})

target_link_libraries(batch-test transaction-tester transaction-tester-algorithm argument-parser)
