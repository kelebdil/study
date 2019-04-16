#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <queue>

#include "base_node.h"


template <
    typename KeyType,
    typename Allocator = std::allocator<KeyType>
    >
class NodePolicyUsePointer
{
public:
    class Node;
    static constexpr Node * node_sentinel = nullptr;

public:
    typedef KeyType key_type;
    class Node : public detail::BaseNode<Node, key_type, Node*, node_sentinel>
    {
        typedef detail::BaseNode<Node, key_type, Node*, node_sentinel> parent_type;
    public:
        Node() = default;
        Node(key_type key) : parent_type(std::move(key)) {}

    };

    typedef Allocator allocator_type;
    typedef typename allocator_type::template rebind<Node>::other node_allocator_type;

    typedef Node node_type;
    typedef node_type * link_type;
    typedef node_type const * const_link_type;

    NodePolicyUsePointer(node_allocator_type node_alloc = node_allocator_type()) :
        node_allocator(node_alloc)
    {
    }

    node_allocator_type & get_allocator() {
        return node_allocator;
    }

    Node & deref(link_type link) const {
        return *link;
    }

    const Node & deref(const_link_type link) const {
        return *link;
    }

    link_type new_node(key_type key) {
        link_type p = node_allocator.allocate(1);
        node_allocator.construct(p, std::move(key));
        return p;
    }

    void deallocate_node(link_type p) {
        node_allocator.destroy(p);
        node_allocator.deallocate(p, 1);
    }


private:
    node_allocator_type node_allocator;
};


template <
    typename KeyType,
    typename Comparator=std::less<KeyType>,
    typename Allocator = std::allocator<KeyType>,
    typename NodePolicy = NodePolicyUsePointer<KeyType, Allocator>
    >
class BinTree
{
public:
    typedef KeyType key_type;
    typedef Comparator comparator_type;
    typedef Allocator allocator_type;
    typedef NodePolicy node_policy_type;

public:
    typedef typename node_policy_type::node_type node_type;
    typedef typename node_type::link_type link_type;
    typedef typename node_policy_type::const_link_type const_link_type;
    static constexpr link_type sentinel = node_type::sentinel;

public:
    BinTree(
            comparator_type comp = comparator_type(),
            allocator_type alloc = allocator_type(),
            node_policy_type node_pol = node_policy_type()
           ) :
        allocator(alloc),
        comparator(comp),
        node_policy(node_pol),
        root()
    {
    }

    ~BinTree(){
        typedef typename allocator_type::template rebind<link_type>::other link_type_allocator;
        std::queue<link_type, std::deque<link_type, link_type_allocator>> q;
        q.push(root);
        while(!q.empty()) {
            link_type p = q.front();
            q.pop();
            node_type & node = node_policy.deref(p);
            if (node.left() != sentinel) {
                q.push(node.left());
            }
            if (node.right() != sentinel) {
                q.push(node.right());
            }
            node_policy.deallocate_node(p);
        }
    }

    allocator_type get_allocator() const {
        return allocator;
    }

    comparator_type get_comparator() const {
        return comparator;
    }

    node_policy_type get_node_policy() const {
        return node_policy;
    }

    const_link_type begin() const {
        return get_directmost_neighbour<detail::left>(root);
    }

    const_link_type rbegin() const {
        return get_directmost_neighbour<detail::right>(root);
    }

    link_type begin() {
        return get_directmost_neighbour<detail::left>(root);
    }

    link_type rbegin() {
        return get_directmost_neighbour<detail::right>(root);
    }

    const_link_type end() const {
        return sentinel;
    }

    const_link_type rend() const {
        return sentinel;
    }

    link_type end() {
        return sentinel;
    }

    link_type rend() {
        return sentinel;
    }

    template <std::size_t direction>
    const_link_type get_directmost_neighbour(const_link_type item) const {
        check_direction<direction>();
        const_link_type current_link = item;
        if (item == sentinel) {
            return sentinel;
        }
        for(;;) {
            link_type next = deref_link<direction>(current_link);
            if (next != sentinel) {
                if (deref_link<direction>(next) == sentinel) {
                    return next;
                }
                current_link = next;
                continue;
            }
            return sentinel;
        }

    }

    template <std::size_t direction>
    link_type get_directmost_neighbour(link_type item) {
        check_direction<direction>();
        link_type current_link = item;
        if (item == sentinel) {
            return sentinel;
        }
        for(;;) {
            link_type next = deref_link<direction>(current_link);
            if (next != sentinel) {
                current_link = next;
                continue;
            }
            return current_link;
        }
        return sentinel;

    }

    std::pair<link_type, bool> insert(key_type key) {
        return insert_at(root, std::move(key));
    }

    template <std::size_t direction>
    const_link_type& deref_link(const_link_type item) const {
        check_direction<direction>();
        if (item == sentinel) {
            return sentinel;
        }
        return node_policy.deref(item).links()[direction];
    }

    template <std::size_t direction>
    link_type& deref_link(link_type item) {
        check_direction<direction>();
        if (item == sentinel) {
            throw std::logic_error("");
        }
        return node_policy.deref(item).links()[direction];
    }

    template <std::size_t Right>
    link_type get_nearest_neighbour(link_type item) {
        check_direction<Right>();
        if (item == sentinel) {
            return sentinel;
        }
        constexpr std::size_t Up = detail::up;
        constexpr std::size_t Left = detail::swap_left_right<Right>();
        if (deref_link<Right>(item) != sentinel) {
            auto result = get_directmost_neighbour<Left>(deref_link<Right>(item));
            return result;
        }
        link_type up = deref_link<Up>(item);
        if (up != sentinel && item == deref_link<Left>(up)) {
            return up;
        }
        while (up != sentinel && item == deref_link<Right>(up)) {
            item = up;
            up = deref_link<Up>(up);
        }
        return up;
    }

protected:
    template <std::size_t direction>
    void check_direction()
    {
        static_assert(
            direction == detail::left ||
            direction == detail::right ||
            direction == detail::up,
        "direction must be left, right or up");
    };

    bool less(key_type k1, key_type k2) const {
        return comparator(k1, k2);
    }

    bool greater(key_type k1, key_type k2) const {
        return comparator(k2, k1);
    }

    bool equal(key_type k1, key_type k2) const {
        return !less(k1, k2) && !greater(k1, k2);
    }

    std::pair<link_type, bool> insert_at(link_type & at, key_type key) {
        link_type *cur_link = &at;
        link_type prev = sentinel;
        for(;;) {

            if (*cur_link == sentinel) {
                *cur_link = node_policy.new_node(std::move(key));
                node_policy.deref(*cur_link).up() = prev;
                return std::make_pair(*cur_link, true);
            } else {
                node_type & cur_node = node_policy.deref(*cur_link);
                if (less(key, cur_node.key())) {
                    prev = *cur_link;
                    cur_link = &cur_node.left();
                    continue;
                } else if (greater(key, cur_node.key())) {
                    prev = *cur_link;
                    cur_link = &cur_node.right();
                    continue;
                } else {
                    return std::make_pair(*cur_link, false);
                }
            }
        }
    }

public:
    allocator_type allocator;
    comparator_type comparator;
    node_policy_type node_policy;
    link_type root;
};
