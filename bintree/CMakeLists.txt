cmake_minimum_required(VERSION 3.20)
project(bintreetests LANGUAGES C CXX)

set(
    SRC
    base_node.h
    bintree.h
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
