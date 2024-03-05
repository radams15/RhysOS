cmake_minimum_required(VERSION 3.26)
project(malloc C)

set(CMAKE_C_STANDARD 11)

add_executable(malloc main.c
        malloc.c
        malloc.h)
