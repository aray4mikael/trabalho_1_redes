cmake_minimum_required(VERSION 3.22)
project(servidor C)

set(CMAKE_C_STANDARD 99)
#set gcc compiler flags -lwsock32 -lws2_32
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lwsock32 -lws2_32")
#set gcc as compiler
set(CMAKE_C_COMPILER "gcc")

add_executable(servidor main-old.c)
