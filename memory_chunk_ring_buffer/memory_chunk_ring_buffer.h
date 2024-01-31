#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

template<typename Allocator = std::allocator<std::byte>>
class memory_chunk_ring_buffer
{
public:
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using byte_allocator = typename allocator_type::template rebind<std::byte>::other;

    memory_chunk_ring_buffer(size_type capacity, allocator_type allocator = allocator_type{})
        : allocator_{ allocator }
        , byte_allocator_{ allocator_ }
        , capacity_{ capacity }
        , head_{ 0 }
        , tail_{ capacity_ }
        , empty_{ true }
        , full_{ false }
        , chunk_count_(0) {
        memory_ = byte_allocator_.allocate(capacity_);
    }

    ~memory_chunk_ring_buffer() { byte_allocator_.deallocate(memory_, capacity_); }

    void * allocate(std::size_t n) {
        std::byte * result = nullptr;
        if (empty_ || !full_) {
            result = memory_ + cheap_mod_capacity(head_);
            std::size_t requested_size = get_aligned_by_size(n + sizeof(std::size_t));
            if (result + requested_size <= memory_ + capacity_) {
                result = memory_ + increment_by_and_check(head_, requested_size);
                *reinterpret_cast<std::size_t *>(result) = cheap_mod_capacity(n);
                result += sizeof(std::size_t);
            } else {
                throw std::overflow_error("chunk ring overflow");
            }
            if (empty_) {
                empty_ = false;
            }
            if (tail_ == head_) {
                full_ = true;
            }
            ++chunk_count_;
            return result;
        } else if (full()) {
            throw std::overflow_error("chunk ring is full");
        }
        __builtin_unreachable();
    }

    void * last_chunk() const {
        if (!empty()) {
            std::byte * p = memory_ + cheap_mod_capacity(tail_ + sizeof(std::size_t));
            return p;
        } else {
            throw std::underflow_error("chunk ring underflow");
        }
    }

    std::size_t size_of_chunk(void * ptr) {
        std::byte * p = reinterpret_cast<std::byte *>(ptr);
        p -= sizeof(std::size_t);
        return *reinterpret_cast<std::size_t *>(p);
    }

    void deallocate(void * pointer) {
        char * ptr = reinterpret_cast<char *>(pointer);
        if (full() || !empty()) {
            std::byte * p = reinterpret_cast<std::byte *>(ptr - sizeof(std::size_t));
            std::size_t data_size = *reinterpret_cast<std::size_t *>(p);
            std::size_t size_to_dealloc = get_aligned_by_size(data_size + sizeof(std::size_t));
            if ((memory_ + cheap_mod_capacity(tail_)) == p) {
                increment_by_and_check(tail_, size_to_dealloc);
                --chunk_count_;
            } else {
                throw std::logic_error("trying to deallocate not last chunk");
            }
            if (full()) {
                full_ = false;
            }
            if (tail_ == head_) {
                empty_ = true;
                head_ = 0;
                tail_ = capacity_;
            }
        } else if (empty()) {
            throw std::underflow_error("chunk ring underflow");
        }
        __builtin_unreachable();
    }

    size_type size() const {
        if (full_) {
            return capacity_;
        } else if (empty()) {
            return 0;
        } else if (head_ > tail_)
            return head_ - tail_;
        else if (head_ < tail_)
            return head_ + (capacity_ - tail_);
        __builtin_unreachable();
    }

    size_type reminder() const {
        std::size_t rem = capacity_ - size();
        std::size_t szofszt = sizeof(std::size_t);
        return rem >= szofszt ? rem - szofszt : 0;
    }

    size_type capacity() const { return capacity_; }

    bool empty() const { return empty_; }

    bool full() const { return full_; }

    size_type chunk_count() const { return chunk_count_; }

private:
    std::size_t get_aligned_by_size(std::size_t n) {
        return (n + sizeof(std::size_t) - 1) & ~std::uint64_t(sizeof(std::size_t) - 1);
    }

    size_type increment_by_and_check(size_type & cursor, size_type delta) {
        size_type result = cursor;
        cursor += delta;
        normalize_idx();
        return cheap_mod_capacity(result);
    }

    size_type cheap_mod_capacity(size_type x) const {
        if (x >= capacity_)
            return x - capacity_;
        return x;
    }

    void normalize_idx() {
        tail_ = cheap_mod_capacity(tail_);
        head_ = cheap_mod_capacity(head_);
    }

private:
    allocator_type allocator_;
    byte_allocator byte_allocator_;
    size_type const capacity_;
    std::byte * memory_;
    size_type head_;
    size_type tail_;
    bool empty_;
    bool full_;
    size_type chunk_count_;
};
