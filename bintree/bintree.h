#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <queue>
#include <iostream>
#include <sstream>
#include <string>

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
        node_allocator.construct(p, key);
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
        std::queue<link_type> q;
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

    template <std::size_t direction>
    const_link_type get_directmost_neighbour(const_link_type item) const {
        static_assert(
            direction == detail::left ||
            direction == detail::right ||
            direction == detail::up,
            "direction must be left, right or up");
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
        static_assert(
            direction == detail::left ||
            direction == detail::right ||
            direction == detail::up,
            "direction must be left, right or up");
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
        static_assert(
            direction == detail::left ||
            direction == detail::right ||
            direction == detail::up,
            "direction must be left, right or up");
        if (item == sentinel) {
            return sentinel;
        }
        return node_policy.deref(item).links()[direction];
    }

    template <std::size_t direction>
    link_type& deref_link(link_type item) {
        static_assert(
            direction == detail::left ||
            direction == detail::right ||
            direction == detail::up,
            "direction must be left, right or up");
        if (item == sentinel) {
            throw std::logic_error("");
        }
        return node_policy.deref(item).links()[direction];
    }

    template <std::size_t Right>
    link_type get_nearest_neighbour(link_type item) {
        static_assert(
            Right == detail::left ||
            Right == detail::right ||
            Right == detail::up,
            "direction must be left, right or up");
        if (item == sentinel) {
            std::cout << "- " ;
            return sentinel;
        }
        constexpr std::size_t Up = detail::up;
        constexpr std::size_t Left = detail::swap_left_right<Right>();
        if (deref_link<Right>(item) != sentinel) {
            auto result = get_directmost_neighbour<Left>(deref_link<Right>(item));
            std::cout << result << " ";
            return result;
        }
        link_type up = deref_link<Up>(item);
        if (up != sentinel && item == deref_link<Left>(up)) {
            std::cout << up << " ";
            return up;
        }
        while (up != sentinel && item == deref_link<Right>(up)) {
            item = up;
            up = deref_link<Up>(up);
        }
        std::cout << up << " ";
        return up;
    }

    std::string dump_tree() const
    {
        std::stringstream ss;

        ss << "+++++DUMP+++++" << std::endl;
        std::queue<link_type> q;
        q.push(root);
        int max_level_size = 1;
        int level_size = max_level_size;
        while(!q.empty()) {
            link_type l = q.front();
            q.pop(); level_size--;
            bool e = (level_size == 0);

            if (l!=sentinel) {
                node_type &node = node_policy.deref(l);
                ss << l;
                q.push(node.left());
                q.push(node.right());
            } else {
                ss << "-";
            }
            if (e) {
                ss << "\n";
                max_level_size *= 2;
                level_size = max_level_size;
            } else {
                ss << " ";
            }
        }
        ss << std::endl;
        ss << "++++++++++++++" << std::endl;
        return ss.str();
    }

protected:

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
//         std::cout << "\n\tkey = " << key << std::endl;
        for(;;) {

            if (*cur_link == sentinel) {
                *cur_link = node_policy.new_node(std::move(key));
                node_policy.deref(*cur_link).up() = prev;
//                 std::cout << " inserted at " << *cur_link << std::endl;
                return std::make_pair(*cur_link, true);
            } else {
                node_type & cur_node = node_policy.deref(*cur_link);
//                 std::cout << "node.key " << cur_node.key() << " ";
                if (less(key, cur_node.key())) {
//                     std::cout << " go left ";
                    prev = *cur_link;
                    cur_link = &cur_node.left();
                    continue;
                } else if (greater(key, cur_node.key())) {
//                     std::cout << " go right ";
                    prev = *cur_link;
                    cur_link = &cur_node.right();
                    continue;
                } else {
//                     std::cout << " already exist" << std::endl;
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
