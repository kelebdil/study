set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -g")

add_executable(memory_chunk_ring_buffer_test
    main.cpp
)

