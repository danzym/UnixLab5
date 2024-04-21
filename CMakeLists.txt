cmake_minimum_required(VERSION 3.26)
project(UnixLab5 C)

set(CMAKE_C_STANDARD 23)

add_executable(UnixLab5 main.c
        server.c)
