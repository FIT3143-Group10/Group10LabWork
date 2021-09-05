cmake_minimum_required(VERSION 3.20)
project(mutex C)

set(CMAKE_C_STANDARD 99)
include_directories(/home/penn/.openmpi/include)

add_executable(mutex main.c IntQueue.c IntQueue.h)
