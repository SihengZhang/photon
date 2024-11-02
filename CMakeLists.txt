cmake_minimum_required(VERSION 3.29)
project(photon)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("/utf-8")
endif()

# Define tinyobjloader as an interface library
add_library(tinyobjloader STATIC externals/tinyobjloader/tiny_obj_loader.cc)
target_include_directories(tinyobjloader PUBLIC externals/tinyobjloader)

add_library(spdlog STATIC externals/spdlog/src/spdlog.cpp)
target_compile_definitions(spdlog PRIVATE SPDLOG_COMPILED_LIB)
target_include_directories(spdlog PUBLIC externals/spdlog/include)

add_executable(photon_project main.cpp)

target_link_libraries(photon_project PRIVATE tinyobjloader)
target_link_libraries(photon_project PRIVATE spdlog)