#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "base_node.h"



template <
    typename KeyType,
    typename NodeType,
    typename Comparator=std::less<KeyType>,
    typename Allocator = std::allocator<KeyType>
    >
class BinTree
{
public:
    typedef KeyType key_type;
    typedef Comparator comparator_type;
    typedef Allocator allocator_type;

private:
    typedef NodeType node_type;
    typedef typename allocator_type::template rebind<node_type>::other node_allocator_type;
    typedef typename node_type::link_type link_type;
    static constexpr link_type sentinel = node_type::sentinel;

public:

public:
    BinTree(comparator_type comp = comparator_type(), allocator_type alloc = allocator_type()) :
        allocator(alloc),
        node_allocator(),
        comparator(comp),
        root()
    {
    }

    allocator_type get_allocator() const {
        return allocator;
    }

    comparator_type get_comparator() const {
        return comparator;
    }

    template <std::size_t direction>
    link_type get_directmost_neighbour(link_type item) {
        static_assert(direction == detail::left || direction == detail::right || direction == detail::up, "");
        link_type current_link = item;
        while (deref(item).links()[direction] != sentinel) {
            current_link = deref(item).links()[direction];
        }

    }

    virtual  node_type & deref(link_type link) const {
        return *node_allocator.address(*link);
    }


private:
    allocator_type allocator;
    node_allocator_type node_allocator;
    comparator_type comparator;
    link_type root;
};
