cmake_minimum_required(VERSION 3.24)
project(AutoVista LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw glm)
find_package(OpenGL REQUIRED)

add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_executable(AutoVista
    src/main.cpp
    src/AvtoVista.cpp
    src/OrbitCamera.cpp
    src/CameraControls.cpp
    src/Texture2D.cpp)
target_link_libraries(AutoVista PRIVATE glfw glm::glm glad OpenGL::GL)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shaders" AUTOVISTA_SHADER_DIR)
target_compile_definitions(AutoVista PRIVATE
    AUTOVISTA_SHADER_DIR="${AUTOVISTA_SHADER_DIR}")
set_property(TARGET AutoVista PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}")

if(MSVC)
    target_compile_options(AutoVista PRIVATE /W4 /permissive-)
else()
    target_compile_options(AutoVista PRIVATE -Wall -Wextra -Wpedantic)
endif()
