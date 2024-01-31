cmake_minimum_required(VERSION 3.20)
project(memory_chunk_ring_buffer LANGUAGES C CXX)

set(
    SRC
    main.cpp
    memory_chunk_ring_buffer.h
)

add_executable(${PROJECT_NAME} ${SRC})
