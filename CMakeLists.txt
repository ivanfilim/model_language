cmake_minimum_required(VERSION 3.10)
project(PreStandardCxx)

# Compilator setup for minimization of the contemporary behaviour
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++98 -fno-rtti -fno-exceptions -fpermissive")

# Adding executable file
add_executable(pre_standard main.cpp)
