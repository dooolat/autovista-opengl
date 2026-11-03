cmake_minimum_required(VERSION 3.24)
project(AutoVista LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

add_executable(AutoVista src/main.cpp)
target_link_libraries(AutoVista PRIVATE OpenGL::GL)
