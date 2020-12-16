project(transaction-tester CXX)

set(CMAKE_CXX_FLAGS "-Wall -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "*.cc" "**/*.cc")
add_executable(transaction-tester ${SRC})

target_link_libraries(transaction-tester pthread argument-parser)
