project(transaction-tester CXX)

set(CMAKE_CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "*.cc" "**/*.cc")
add_library(transaction-tester-algorithm SHARED ${SRC})

target_link_libraries(transaction-tester-algorithm argument-parser pthread)
