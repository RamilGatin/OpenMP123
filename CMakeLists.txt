cmake_minimum_required(VERSION 3.23)
project (OpenMP123)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

add_executable(OpenMP123 main.cpp)
