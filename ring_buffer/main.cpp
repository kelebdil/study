#include "ring_buffer.h"

#include <iostream>

constexpr std::size_t max_arena_size = 100ULL * 1024 * 1024;

struct test_ring
{
    test_ring() = default;

    ~test_ring() = default;

    template<typename T>
    void test(std::size_t prefill_items, T (*factory)(std::size_t)) {
        ring_buffer<T> ring(max_arena_size / sizeof(T));

        for (std::size_t i = 0; i < prefill_items; ++i) {
            ring.push((*factory)(i));
            if (i % 1024 == 0) {
                std::cout << "Size: " << ring.size() << std::endl;
            }
        }

        for (std::size_t i = 0; i < 10 * ring.capacity(); ++i) {
            __attribute__((unused)) auto v = ring.pop();
            if (i % 1024 == 0) {
                std::cout << "Size: " << ring.size() << std::endl;
            }
            ring.push((*factory)(i));
        }

        std::cout << "Size: " << ring.size() << std::endl;
    }
};

int main(int, char **) {
    auto int_factory = [](std::size_t i) -> int { return i; };

    {
        test_ring tr;
        tr.test<int>(max_arena_size / sizeof(int), +int_factory);
    }

    {
        test_ring tr;
        tr.test<int>(1, +int_factory);
    }

    {
        test_ring tr;
        tr.test<int>(max_arena_size / sizeof(int) / 2, +int_factory);
    }

    auto string_factory = [](std::size_t i) -> std::string {
        return std::string{ "some_preffix" } + std::to_string(i);
    };

    {
        test_ring tr;
        tr.test<std::string>(max_arena_size / sizeof(std::string), +string_factory);
    }

    return 0;
}
