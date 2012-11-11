cmake_minimum_required(VERSION 2.8)

project(CVRPTW C)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")

SET(SRC_LIST main.c)

add_executable(cvrptw ${SRC_LIST})

find_library(M_LIB m)
find_package (Threads)
target_link_libraries(cvrptw ${M_LIB} ${CMAKE_THREAD_LIBS_INIT})
