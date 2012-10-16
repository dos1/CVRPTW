cmake_minimum_required(VERSION 2.8)

project(CVRPTW C)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")

SET(SRC_LIST main.c)

add_executable(cvrptw ${SRC_LIST})
