cmake_minimum_required(VERSION 3.6)
project(Chat)

set(CMAKE_CXX_COMPILER 11)
find_package(Threads)
set(SERVER server_side.c)
set(CLIENT client_side.c)

add_library(HEADER_FILES  SHARED common.c common.h)

add_executable(server ${SERVER})
add_executable(client ${CLIENT})

target_link_libraries(server  PRIVATE ${CMAKE_THREAD_LIBS_INIT} HEADER_FILES)
target_link_libraries(client PRIVATE ${CMAKE_THREAD_LIBS_INIT} HEADER_FILES)