#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>


template <typename T, typename Allocator = std::allocator<T>>
class ring_buffer
{
public:
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef std::size_t size_type;

    size_type capacity() const {
        return capacity_;
    }

    ring_buffer(size_type capacity, allocator_type allocator = allocator_type()) :
        allocator_(allocator),
        capacity_(capacity),
        buffer_(capacity_, value_type(), allocator_),
        head_(0),
        tail_(capacity_),
        empty_(true),
        full_(false)
    {
    }

    void push(value_type v) {
        if (empty() || !full()) {
            buffer_[increment_and_check(head_)] = std::move(v);
            if (empty()) {
                empty_ = false;
            }
            if (tail_ == head_) {
                full_ = true;
            }
        } else if (full()) {
            throw std::overflow_error("ring overflow");
        }
    }

    value_type pop() {
        if (full() || !empty()) {
            value_type result = std::move(buffer_[increment_and_check(tail_)]);
            if (full()) {
                full_ = false;
            }
            if (tail_ == head_) {
                empty_ = true;
            }
            return result;

        } else if (empty()) {
            throw std::underflow_error("ring underflow");
        }
    }

    size_type size() const {
        if (full()) {
            return capacity_;
        } else if (empty()) {
            return 0;
        } else if (head_ > tail_)
            return head_ - tail_;
        else if (head_ < tail_)
            return head_ + (capacity_ - tail_);
    }

    bool empty() const {
        return empty_;
    }

    bool full() const {
        return full_;
    }

private:
    size_type increment_and_check(size_type &cursor) {
        size_type result = cursor++;
        normalize_idx();
        return cheap_mod_capacity(result);
    }

    size_type cheap_mod_capacity(size_type x)
    {
        if (x >= capacity_)
            return x - capacity_;
        return x;
    }
    void normalize_idx()
    {
        tail_ = cheap_mod_capacity(tail_);
        head_ = cheap_mod_capacity(head_);
    }
private:
    allocator_type allocator_;
    const size_type capacity_;
    std::vector<T, allocator_type> buffer_;
    size_type head_;
    size_type tail_;
    bool empty_;
    bool full_;
};
