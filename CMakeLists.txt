cmake_minimum_required(VERSION 3.6)
project(Chat)

set(CMAKE_CXX_COMPILER 11)
find_package (Threads)
set(SERVER server_side.c)
set(CLIENT t_c.c)

add_executable(server ${SERVER})
add_executable(client ${CLIENT})

target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})