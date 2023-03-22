cmake_minimum_required(VERSION 3.16)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Apollo_CPP_Client)

add_subdirectory(src)


add_executable(example main.cpp)