cmake_minimum_required(VERSION 3.12)
project(opengl_demo)

set(CMAKE_CXX_STANDARD 11)

link_directories("/usr/local/lib")
link_libraries(GL glut GLEW OpenMeshCore)

add_executable(opengl_demo main.cpp shader.h)