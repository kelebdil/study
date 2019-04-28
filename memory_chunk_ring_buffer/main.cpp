#include "memory_chunk_ring_buffer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

constexpr std::size_t max_arena_size = 2*1024ULL*1024ULL*1024ULL;

struct test_chunk_ring
{
    test_chunk_ring() :
        chunk_ring(max_arena_size)
    {
    }

    ~test_chunk_ring()
    {
    }


    void test() {

        std::mt19937 prng{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> dist(1, 65535);

        std::size_t chunk_counter = 0;
        for (std::size_t i = 0; !chunk_ring.full(); ++i) {
            try {
                char *data;
                std::size_t size = dist(prng);
//                 std::cout << "trying to allocate " << size << " bytes" << std::endl;
                data = (char *)chunk_ring.allocate(size);
                std::size_t real_size = chunk_ring.size_of_chunk(data);
                assert(size <= real_size);
//                 std::cout << "size: " << size << ", real_size: " << real_size << ", data: " << (void*)data << std::endl;
                //std::fill_n(data, real_size, '\0');
//                 std::cout << "size: " << size << ", real_size: " << real_size << ", data: " << (void*)data << std::endl;
                chunk_counter++;
            } catch (const std::overflow_error &e) {
                std::cout << "overflow_error" << std::endl;
                std::size_t delta = chunk_ring.reminder();
                if (delta > 0) {
                    std::size_t size = delta;
                    try {
                        char *data = (char *)chunk_ring.allocate(size);
                        std::size_t real_size = chunk_ring.size_of_chunk(data);
                        std::cout << "*size: " << size << ", real_size: " << real_size << ", data: " << (void*)data << std::endl;
                        //std::fill_n(data, real_size, '\0');
                        std::cout << "*size: " << size << ", real_size: " << real_size << ", data: " << (void*)data << std::endl;
                        chunk_counter++;
                    } catch (...) {
                        break;
                    }
                }
            }
            if (i % 1024 == 0) {
                std::cout << "Size: " << chunk_ring.size() << std::endl;
            }
        }

        std::cout << "chunk_counter: " << chunk_counter << std::endl;


        for(std::size_t i = 0; i < chunk_counter * 10; ++i) {
            if (!chunk_ring.empty()) {
                char * chunk = (char *)chunk_ring.last_chunk();
                std::size_t size = chunk_ring.size_of_chunk(chunk);
                //std::cout << "last_chunk size: " << size << ", data: " << chunk << std::endl;
//                 std::size_t ensure = std::count_if(chunk, chunk + size, [](const char &c){ return c != 0;});
//                 if (ensure) {
//                     std::cout << "Extra not null bytes " << ensure << std::endl;
//                 }
                chunk_ring.deallocate(chunk);
            } else {
                std::cout << "chunk ring empty" << std::endl;
            }

            try {
                std::uniform_int_distribution<std::size_t> dist(1, chunk_ring.reminder() - 2*sizeof(std::size_t));
                char * data;
                std::size_t size = dist(prng);
                data = (char*)chunk_ring.allocate(size);
                std::size_t real_size = chunk_ring.size_of_chunk(data);
                //std::cout << "size: " << size << ", real_size: " << real_size << ", data: " << (void*)data << std::endl;
                assert(size <= real_size);
                //std::fill_n(data, real_size, '\0');
            } catch (const std::overflow_error &e) {
                if (!chunk_ring.full()) {
                    std::size_t delta = chunk_ring.reminder();
                    if (delta > 0) {
                        char* data;
                        std::size_t size = delta;
                        try {
                            data = (char*)chunk_ring.allocate(size);
                            std::size_t real_size = chunk_ring.size_of_chunk(data);
                            //std::fill_n(data, real_size, 0);
                        } catch (...) {
                            break;
                        }
                    }
                }
            }

            if (i % 1024 == 0) {
                std::cout << "Size: " << chunk_ring.size() << std::endl;
            }
        }

        std::cout << "Size: " << chunk_ring.size() << std::endl;

        while (!chunk_ring.empty()) {
            char * chunk = (char *)chunk_ring.last_chunk();
            std::size_t size = chunk_ring.size_of_chunk(chunk);
            //std::cout << "last_chunk size: " << size << ", data: " << (void*)chunk << std::endl;
//             std::size_t ensure = std::count_if(chunk, chunk + size, [](const char &c){ return c != 0;});
//             if (ensure) {
//                 std::cout << "Extra not null bytes " << ensure << std::endl;
//             }
            chunk_ring.deallocate(chunk);
        }

        std::cout << "Size: " << chunk_ring.size() << std::endl;
    }

private:
    memory_chunk_ring_buffer<> chunk_ring;
};

int main(int , char ** )
{
    {
        test_chunk_ring tr;
        tr.test();
        std::cout << "=============================================================" << std::endl;
        tr.test();
    }

    return 0;
}
