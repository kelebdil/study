#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>


template <typename Allocator = std::allocator<char>>
class memory_chunk_ring_buffer
{
public:
    typedef Allocator allocator_type;
    typedef std::size_t size_type;
    typedef typename allocator_type::template rebind<char>::other char_allocator;

    size_type capacity() const {
        return capacity_;
    }

    memory_chunk_ring_buffer(size_type capacity, allocator_type allocator = allocator_type()) :
        allocator_(allocator),
        char_allocator_(allocator_),
        capacity_(capacity),
        head_(0),
        tail_(capacity_),
        empty_(true),
        full_(false),
        chunk_count_(0)
    {
        memory_ = char_allocator_.allocate(capacity_);
    }

    ~memory_chunk_ring_buffer()
    {
        char_allocator_.deallocate(memory_, capacity_);
    }

    void* allocate(std::size_t n) {
        char * result = nullptr;
        if (empty_ || !full_) {
            std::size_t requested_size = (n + 2*sizeof(std::size_t) - 1) & ~std::uint64_t(sizeof(std::size_t) -1);
            result = memory_ + cheap_mod_capacity(head_);
            if (result + requested_size <= memory_ + capacity_) {
                result = memory_ + increment_by_and_check(head_, requested_size);
                *reinterpret_cast<std::size_t *>(result) = cheap_mod_capacity(requested_size - sizeof(std::size_t));
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
    }

    void* last_chunk() const
    {
        if (!empty()) {
            char *p = memory_ + cheap_mod_capacity(tail_ + sizeof(std::size_t));
            return p;
        } else {
            throw std::underflow_error("chunk ring underflow");
        }
    }

    std::size_t size_of_chunk(void *ptr) {
        char * p = reinterpret_cast<char *>(ptr);
        p -= sizeof(std::size_t);
        return *reinterpret_cast<std::size_t *>(p);
    }

    void deallocate(void *pointer) {
        char * ptr = reinterpret_cast<char *>(pointer);
        if (full() || !empty()) {
            char *p = ptr - sizeof(std::size_t);
            std::size_t data_size = *reinterpret_cast<std::size_t *>(p);
            if ((memory_ + cheap_mod_capacity(tail_)) == p) {
                increment_by_and_check(tail_, data_size + sizeof(std::size_t));
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
        } else  if (empty()) {
            throw std::underflow_error("chunk ring underflow");
        }
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
    }

    size_type reminder() const {
        std::size_t rem = capacity_ - size() ;
        std::size_t szofszt = sizeof(std::size_t);
        return rem >= szofszt ? rem - szofszt : 0;
    }

    bool empty() const {
        return empty_;
    }

    bool full() const {
        return full_;
    }

    size_type chunk_count() const {
        return chunk_count_;
    }

private:
    size_type increment_by_and_check(size_type &cursor, size_type delta) {
        size_type result = cursor;
        cursor += delta;
        normalize_idx();
        return cheap_mod_capacity(result);
    }

    size_type cheap_mod_capacity(size_type x) const
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
    char_allocator char_allocator_;
    const size_type capacity_;
    char *memory_;
    size_type head_;
    size_type tail_;
    bool empty_;
    bool full_;
    size_type chunk_count_;
};
