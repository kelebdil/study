cmake_minimum_required(VERSION 3.20)
project(brackets LANGUAGES C CXX)

set(
    SRC
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
