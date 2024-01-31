cmake_minimum_required(VERSION 3.20)
project(external_sort LANGUAGES C CXX)

set(
    SRC
    external_sort.hpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
