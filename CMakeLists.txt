cmake_minimum_required(VERSION 3.29)
project(photon_project)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals/embree)

add_executable(photon_project main.cpp)

target_link_libraries(photon_project PRIVATE embree3)