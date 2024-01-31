#include "external_sort.hpp"

#include <iostream>
#include <memory>
#include <random>

/*
 * Testing:
 * ./external_sort > test.txt
 * (for j in $(for i in $(head -n 1 test.txt); do echo $i; done | sort -n ); do echo -n $j; \
 * echo -n " "; done; echo "";) | sha256sum
 * tail -n 1 test.txt | sha256sum
 */

template<typename Value>
class VectorTape : public external_sort::Tape<Value>
{
public:
    using value_type = Value;
    VectorTape(size_t maxSize) { data.reserve(maxSize); }
    VectorTape(std::vector<Value> source)
        : data{ std::move(source) } {}

    std::size_t pos() const override { return cursor; }
    void pos(std::size_t idx) override { cursor = idx; }
    value_type read() const override {
        if (cursor >= data.size())
            throw std::runtime_error("Read out of data bounds");
        return data[cursor];
    }

    std::size_t size() const override { return data.size(); }
    std::size_t capacity() const override { return data.capacity(); }

    void write(value_type value) override {
        if (cursor >= data.capacity())
            throw std::runtime_error("Can't write beyond end of data");

        if (cursor >= data.size())
            data.resize(cursor + 1);
        data[cursor] = value;
    }

    void flush() override {}

    std::vector<Value> const & getData() const { return data; }

private:
    std::size_t cursor = 0;
    std::vector<Value> data;
};

int main(int, char **) {
    std::size_t const dataSize = 1024ULL * 1024ULL;
    std::vector<int> data;
    data.resize(dataSize);
    std::random_device rd{};
    std::mt19937 rng{ rd() };
    std::uniform_int_distribution<int> distr(0, 1000);
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = distr(rng);
    }

    for (auto i : data) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::vector<std::unique_ptr<external_sort::Tape<int>>> tmp;

    auto in = std::make_unique<VectorTape<int>>(data);
    auto out = std::make_unique<VectorTape<int>>(dataSize);

    constexpr std::size_t kMaxMemorySize = 1024;
    tmp.reserve(dataSize * sizeof(int) / kMaxMemorySize + 1);
    for (size_t i = 0; i < tmp.capacity(); ++i) {
        tmp.emplace_back();
        tmp.back() = std::make_unique<VectorTape<int>>(kMaxMemorySize / sizeof(int));
    }
    external_sort::externalSort<int, kMaxMemorySize>(in, out, tmp);
    for (auto d : out->getData()) {
        std::cout << d << " ";
    }
    std::cout << std::endl;

    return 0;
}
