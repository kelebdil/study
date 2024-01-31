#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace external_sort
{

template<typename Value>
class Tape
{
public:
    using value_type = Value;
    virtual ~Tape() = default;
    virtual std::size_t pos() const = 0;
    virtual void pos(std::size_t idx) = 0;
    virtual value_type read() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::size_t capacity() const = 0;
    virtual void write(value_type value) = 0;
    virtual void flush() = 0;
};

namespace detail
{

template<typename Value, std::size_t kMaxMemorySize, typename TapePtr = Tape<Value> *,
         typename TapesContainer = std::vector<TapePtr>>
void merge(size_t totalSize, TapesContainer && in, std::vector<std::size_t> const & sizes,
           TapePtr & out) {
    std::vector<std::size_t> cursors;
    cursors.resize(in.size());
    std::multiset<Value> data;
    std::size_t writeCursor = 0;

    constexpr std::size_t kCursorGuardValue = ~std::size_t(0);

    auto getNextNonEmptyInputIndex = [&](std::size_t startIndex) -> std::size_t {
        std::size_t currentIndex = startIndex;
        while (currentIndex < in.size() && cursors[currentIndex] >= in[currentIndex]->size())
            ++currentIndex;
        if (currentIndex == in.size())
            return kCursorGuardValue;
        return currentIndex;
    };

    auto findMinimalValue = [&]() -> std::pair<std::size_t, std::optional<Value>> {
        auto nextNonEmptyIndex = getNextNonEmptyInputIndex(0);

        if (nextNonEmptyIndex == kCursorGuardValue)
            return std::make_pair(kCursorGuardValue, std::nullopt);

        std::size_t minIdx = nextNonEmptyIndex;
        in[minIdx]->pos(cursors[minIdx]);
        Value minValue = in[minIdx]->read();

        while (nextNonEmptyIndex != kCursorGuardValue) {
            nextNonEmptyIndex = getNextNonEmptyInputIndex(nextNonEmptyIndex + 1);
            if (nextNonEmptyIndex == kCursorGuardValue)
                break;
            in[nextNonEmptyIndex]->pos(cursors[nextNonEmptyIndex]);
            Value nextValue = in[nextNonEmptyIndex]->read();
            if (nextValue < minValue) {
                minValue = nextValue;
                minIdx = nextNonEmptyIndex;
            }
        }
        return std::make_pair(minIdx, std::move(minValue));
    };

    using multiset = std::multiset<Value>;
    using node_type = typename multiset::node_type;
    std::optional<node_type> node;
    constexpr std::size_t kMaxMultisetSize = kMaxMemorySize / sizeof(node_type);
    for (;;) {
        auto min = findMinimalValue();
        while (min.first != kCursorGuardValue && data.size() < kMaxMultisetSize) {
            ++cursors[min.first];
            if (node) {
                node->value() = *min.second;
                data.insert(std::move(*node));
                node.reset();
            } else {
                data.insert(*min.second);
            }
            min = findMinimalValue();
        }
        node = data.extract(data.begin());
        out->pos(writeCursor);
        out->write(node->value());
        ++writeCursor;
        if (writeCursor == totalSize)
            break;
    }
    out->flush();
}

}  // namespace detail

template<typename Value, std::size_t kMaxMemorySize, typename TapePtr = Tape<Value> *,
         typename TapesContainer = std::vector<TapePtr>>
void externalSort(TapePtr && in, TapePtr && out, TapesContainer & tmp) {
    std::size_t const inputSize = in->size();
    std::size_t const chunkCount = (sizeof(Value) * inputSize + kMaxMemorySize - 1) / kMaxMemorySize;
    std::size_t const chunkSize = inputSize / chunkCount;

    if (chunkSize > kMaxMemorySize)
        throw std::runtime_error("Insufficient memory");

    std::size_t const tmpCapacity = [&tmp, chunkSize]() {
        std::size_t result = 0;
        for (auto & tmpPtr : tmp) {
            std::size_t cap = tmpPtr->capacity();
            if (cap < chunkSize)
                throw std::runtime_error("One of temporary tapes too small: " + std::to_string(cap)
                                         + " < " + std::to_string(chunkSize));
            result += cap;
        }
        return result;
    }();

    if (tmpCapacity < inputSize)
        throw std::runtime_error("Insufficient temporary space: " + std::to_string(tmpCapacity)
                                 + " < " + std::to_string(inputSize));
    std::vector<std::size_t> chunkSizes;
    chunkSizes.resize(tmp.size());
    std::size_t totalRead = 0;
    std::size_t tmpIndex = 0;

    while (totalRead < inputSize) {
        std::vector<Value> chunk;
        chunk.reserve(chunkSize);
        std::size_t cursor = 0;
        while (cursor < chunkSize && totalRead + cursor < inputSize) {
            in->pos(totalRead + cursor);
            chunk.emplace_back(in->read());
            ++cursor;
        }
        std::sort(chunk.begin(), chunk.end());
        chunkSizes[tmpIndex] = chunk.size();
        totalRead += cursor;
        cursor = 0;
        while (cursor < chunk.size()) {
            tmp[tmpIndex]->pos(cursor);
            tmp[tmpIndex]->write(chunk[cursor]);
            ++cursor;
        }
        tmp[tmpIndex]->flush();
        ++tmpIndex;
    }
    detail::merge<Value, kMaxMemorySize>(inputSize, tmp, chunkSizes, out);
}

}  // namespace external_sort
