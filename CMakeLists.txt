cmake_minimum_required(VERSION 3.29)
project(photon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
#set(CMAKE_TOOLCHAIN_FILE "C:/Users/sihen/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

set(CMAKE_PREFIX_PATH "C:/Users/sihen/vcpkg/installed/x64-windows")

if (MSVC)
    add_compile_options("/utf-8")
endif()

# Define tinyobjloader as a library
add_library(tinyobjloader STATIC externals/tinyobjloader/tiny_obj_loader.cc)
target_include_directories(tinyobjloader PUBLIC externals/tinyobjloader)

# Define spdlog as a library
add_library(spdlog STATIC externals/spdlog/src/spdlog.cpp)
target_compile_definitions(spdlog PRIVATE SPDLOG_COMPILED_LIB)
target_include_directories(spdlog PUBLIC externals/spdlog/include)

# find the other libraries
find_package(OpenMP REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(embree 3.0 REQUIRED)

add_executable(photon main.cpp
        include/camera.h
        include/core.h
        include/image.h
        include/integrator.h
        include/light.h
        include/material.h
        include/photon_map.h
        include/primitive.h
        include/sampler.h
        include/scene.h
        include/triangle.h
        include/json.h
        src/camera.cpp
        src/image.cpp
        src/light.cpp
        src/triangle.cpp
)

target_include_directories(photon PRIVATE ./include)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(photon PRIVATE tinyobjloader spdlog OpenMP::OpenMP_CXX ${OpenCV_LIBS} embree)

