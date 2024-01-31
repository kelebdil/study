cmake_minimum_required(VERSION 3.20)
project(ring_buffer_test LANGUAGES C CXX)

set(
    SRC
    main.cpp
    ring_buffer.h
)

add_executable(${PROJECT_NAME} ${SRC})
