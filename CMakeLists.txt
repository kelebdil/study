cmake_minimum_required(VERSION 3.20)
project(study LANGUAGES C CXX)

message("CMAKE_VERSION: ${CMAKE_VERSION}")

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

check_cxx_compiler_flag("-std=c++17" STDCXX_17_SUPPORTED)
if (NOT STDCXX_17_SUPPORTED)
    message(FATAL_ERROR "C++17 standard required")
endif ()

add_compile_options(
    "-Wall"
    "-O3"
    "-g"
)

add_subdirectory(bintree)
add_subdirectory(brackets)
add_subdirectory(memory_chunk_ring_buffer)
add_subdirectory(ring_buffer)
