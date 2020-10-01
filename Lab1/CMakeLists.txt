cmake_minimum_required(VERSION 3.15)
project(Lab1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Lab1 main.c settings.h utils.h models.h commands.h dbfiles.h indexedtable.h)