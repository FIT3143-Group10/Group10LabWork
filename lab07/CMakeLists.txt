cmake_minimum_required(VERSION 3.20)
project(lab07 C)

set(CMAKE_C_STANDARD 99)

add_executable(task4.out task4.c)
add_executable(task4a.out task4a.c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(task4b.out task4b.c)
