cmake_minimum_required(VERSION 3.16)
project(NetTest)


set(CMAKE_CXX_STANDARD 17)

include_directories("include")

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -ggdb")

add_executable(NetTest
    src/main.cpp
    src/Socket.cpp
    src/LinuxSocket.cpp
)
