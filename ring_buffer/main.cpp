#include "ring_buffer.h"

#include <iostream>

constexpr std::size_t max_arena_size = 100ULL*1024*1024;

struct test_ring
{
    test_ring()
    {
    }

    ~test_ring()
    {
    }

    template <typename T>
    void test(std::size_t prefill_items) {
        ring_buffer<T> ring(max_arena_size / sizeof(T));

        for (std::size_t i = 0; i < prefill_items; ++i) {
            ring.push(T());
            if (i % 1024 == 0) {
                std::cout << "Size: " << ring.size() << std::endl;
            }
        }

        std::size_t i = 1;
        for(std::size_t i = 0; i < 10*ring.capacity(); ++i) {
            auto v = ring.pop();
            if (i % 1024 == 0) {
                std::cout << "Size: " << ring.size() << std::endl;
            }
            ring.push(T());
        }

        std::cout << "Size: " << ring.size() << std::endl;
    }
};

int main(int , char ** )
{
    {
        test_ring tr;
        tr.test<int>(max_arena_size/sizeof(int));
    }

    {
        test_ring tr;
        tr.test<int>(1);
    }

    {
        test_ring tr;
        tr.test<int>(max_arena_size/sizeof(int)/2);
    }
    return 0;
}
