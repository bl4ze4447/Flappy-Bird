cmake_minimum_required(VERSION 3.30)
project(untitled4 C)

set(CMAKE_C_STANDARD 99)

set(raylib /opt/homebrew/Cellar/raylib/5.5)
include_directories(${raylib}/include/)
link_directories(${raylib}/lib/)
add_executable(untitled4 main.c)


target_link_libraries(untitled4 PRIVATE raylib)
