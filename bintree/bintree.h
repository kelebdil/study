#pragma once

#include "base_node.h"

#include <memory>
#include <queue>
#include <utility>

template<typename KeyType, typename Allocator = std::allocator<KeyType>>
class NodePolicyUsePointer
{
public:
    class Node;
    static constexpr Node * node_sentinel = nullptr;

public:
    using key_type = KeyType;

    class Node : public detail::BaseNode<Node, key_type, Node *, node_sentinel>
    {
        using parent_type = detail::BaseNode<Node, key_type, Node *, node_sentinel>;

    public:
        Node() = default;
        Node(key_type key)
            : parent_type{ std::move(key) } {}
    };

    using allocator_type = Allocator;
    using node_allocator_type = typename allocator_type::template rebind<Node>::other;

    using node_type = Node;
    using link_type = node_type *;
    using const_link_type = node_type const *;

    NodePolicyUsePointer(node_allocator_type node_alloc = node_allocator_type())
        : node_allocator{ node_alloc } {}

    node_allocator_type & get_allocator() { return node_allocator; }

    Node & deref(link_type link) const { return *link; }

    const Node & deref(const_link_type link) const { return *link; }

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

template<typename KeyType, typename Comparator = std::less<KeyType>,
         typename Allocator = std::allocator<KeyType>,
         typename NodePolicy = NodePolicyUsePointer<KeyType, Allocator>>
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
    static constexpr link_type sentinel_ = node_type::sentinel;
    link_type sentinel() { return sentinel_; }

    const_link_type sentinel() const { return const_cast<const_link_type>(sentinel_); }

public:
    BinTree(comparator_type comp = comparator_type(), allocator_type alloc = allocator_type(),
            node_policy_type node_pol = node_policy_type())
        : allocator(alloc)
        , comparator(comp)
        , node_policy(node_pol)
        , root() {}

    ~BinTree() {
        typedef typename allocator_type::template rebind<link_type>::other link_type_allocator;
        std::queue<link_type, std::deque<link_type, link_type_allocator>> q;
        q.push(root);
        while (!q.empty()) {
            link_type p = q.front();
            q.pop();
            node_type & node = node_policy.deref(p);
            if (node.left() != sentinel()) {
                q.push(node.left());
            }
            if (node.right() != sentinel()) {
                q.push(node.right());
            }
            node_policy.deallocate_node(p);
        }
    }

    friend class iterator;

    class iterator
    {
        friend class BinTree;
        iterator(link_type link, BinTree * tree)
            : link(link)
            , tree(tree) {}

    public:
        iterator() {}
        iterator(const iterator &) = default;
        iterator(iterator &&) = default;
        iterator & operator=(const iterator &) = default;
        iterator & operator=(iterator &&) = default;

        key_type & operator*() { return tree->node_policy.deref(link).key(); }

        key_type * operator->() { return &tree->node_policy.deref(link).key(); }

        iterator operator++() {
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return *this;
        }

        iterator operator++(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return std::move(result);
        }

        iterator operator--() {
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return *this;
        }

        iterator operator--(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return std::move(result);
        }

        bool operator==(const iterator & it) const { return link == it.link; }

        bool operator!=(const iterator & it) const { return link != it.link; }

    private:
        link_type link;
        BinTree * tree;
    };

    friend class const_iterator;

    class const_iterator
    {
        friend class BinTree;
        const_iterator(const_link_type link, const BinTree * tree)
            : link(link)
            , tree(tree) {}

    public:
        const_iterator() {}
        const_iterator(const const_iterator &) = default;
        const_iterator(const_iterator &&) = default;
        const_iterator & operator=(const const_iterator &) = default;
        const_iterator & operator=(const_iterator &&) = default;

        const_iterator(const iterator & it)
            : link(it.link)
            , tree(it.tree) {}

        const key_type & operator*() { return tree->node_policy.deref(link).key(); }

        const key_type * operator->() { return &tree->node_policy.deref(link).key(); }

        const_iterator operator++() {
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return *this;
        }

        const_iterator operator++(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return std::move(result);
        }

        const_iterator operator--() {
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return *this;
        }

        const_iterator operator--(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return std::move(result);
        }

        bool operator==(const const_iterator & it) const { return link == it.link; }

        bool operator!=(const const_iterator & it) const { return link != it.link; }

    private:
        const_link_type link;
        const BinTree * tree;
    };

    friend class reverese_iterator;

    class reverese_iterator
    {
        friend class BinTree;
        reverese_iterator(link_type link, BinTree * tree)
            : link(link)
            , tree(tree) {}

    public:
        reverese_iterator() {}
        reverese_iterator(const reverese_iterator &) = default;
        reverese_iterator(reverese_iterator &&) = default;
        reverese_iterator & operator=(const reverese_iterator &) = default;
        reverese_iterator & operator=(reverese_iterator &&) = default;

        key_type & operator*() { return tree->node_policy.deref(link).key(); }

        key_type * operator->() { return &tree->node_policy.deref(link).key(); }

        reverese_iterator operator++() {
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return *this;
        }

        reverese_iterator operator++(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return std::move(result);
        }

        reverese_iterator operator--() {
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return *this;
        }

        reverese_iterator operator--(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return std::move(result);
        }

        bool operator==(const reverese_iterator & it) const { return link == it.link; }

        bool operator!=(const reverese_iterator & it) const { return link != it.link; }

    private:
        link_type link;
        BinTree * tree;
    };

    friend class const_reverese_iterator;

    class const_reverese_iterator
    {
        friend class BinTree;
        const_reverese_iterator(const_link_type link, const BinTree * tree)
            : link(link)
            , tree(tree) {}

    public:
        const_reverese_iterator() {}
        const_reverese_iterator(const const_reverese_iterator &) = default;
        const_reverese_iterator(const_reverese_iterator &&) = default;
        const_reverese_iterator & operator=(const const_reverese_iterator &) = default;
        const_reverese_iterator & operator=(const_reverese_iterator &&) = default;

        const_reverese_iterator(const reverese_iterator & it)
            : link(it.link)
            , tree(it.tree) {}

        const key_type & operator*() { return tree->node_policy.deref(link).key(); }

        const key_type * operator->() { return &tree->node_policy.deref(link).key(); }

        const_reverese_iterator operator++() {
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return *this;
        }

        const_reverese_iterator operator++(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Left>(link);
            return std::move(result);
        }

        const_reverese_iterator operator--() {
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return *this;
        }

        const_reverese_iterator operator--(int) {
            auto result = *this;
            link = tree->get_nearest_neighbour<node_type::Right>(link);
            return std::move(result);
        }

        bool operator==(const const_reverese_iterator & it) const { return link == it.link; }

        bool operator!=(const const_reverese_iterator & it) const { return link != it.link; }

    private:
        const_link_type link;
        const BinTree * tree;
    };

    allocator_type get_allocator() const { return allocator; }

    comparator_type get_comparator() const { return comparator; }

    node_policy_type get_node_policy() const { return node_policy; }

    iterator begin() { return iterator(get_directmost_neighbour<node_type::Left>(root), this); }

    const_reverese_iterator rbegin() const {
        return const_reverese_iterator(get_directmost_neighbour<node_type::Right>(root), this);
    }

    const_iterator begin() const {
        return const_iterator(get_directmost_neighbour<node_type::Left>(root), this);
    }

    reverese_iterator rbegin() {
        return reverese_iterator(get_directmost_neighbour<node_type::Right>(root), this);
    }

    const_iterator end() const { return const_iterator(sentinel(), this); }

    const_reverese_iterator rend() const { return const_reverese_iterator(sentinel(), this); }

    iterator end() { return iterator(sentinel(), this); }

    reverese_iterator rend() { return reverese_iterator(sentinel(), this); }

    template<std::size_t direction>
    const_link_type get_directmost_neighbour(const_link_type item) const {
        check_direction<direction>();
        const_link_type current_link = item;
        if (item == sentinel()) {
            return sentinel();
        }
        for (;;) {
            const_link_type next = deref_link<direction>(current_link);
            if (next != sentinel()) {
                if (deref_link<direction>(next) == sentinel()) {
                    return next;
                }
                current_link = next;
                continue;
            }
            return sentinel();
        }
    }

    template<std::size_t direction>
    link_type get_directmost_neighbour(link_type item) {
        check_direction<direction>();
        link_type current_link = item;
        if (item == sentinel()) {
            return sentinel();
        }
        for (;;) {
            link_type next = deref_link<direction>(current_link);
            if (next != sentinel()) {
                current_link = next;
                continue;
            }
            return current_link;
        }
        return sentinel();
    }

    std::pair<link_type, bool> insert(key_type key) { return insert_at(root, std::move(key)); }

    template<std::size_t direction>
    const_link_type deref_link(const_link_type item) const {
        check_direction<direction>();
        if (item == sentinel()) {
            return sentinel();
        }
        return node_policy.deref(item).links()[direction];
    }

    template<std::size_t direction>
    link_type & deref_link(link_type item) {
        check_direction<direction>();
        if (item == sentinel()) {
            throw std::logic_error("");
        }
        return node_policy.deref(item).links()[direction];
    }

    template<std::size_t Right>
    link_type get_nearest_neighbour(link_type item) {
        check_direction<Right>();
        if (item == sentinel()) {
            return sentinel();
        }
        constexpr std::size_t Up = node_type::Up;
        constexpr auto Left = node_type::template swap_left_right<Right>();
        if (deref_link<Right>(item) != sentinel()) {
            auto result = get_directmost_neighbour<Left>(deref_link<Right>(item));
            return result;
        }
        link_type up = deref_link<Up>(item);
        if (up != sentinel() && item == deref_link<Left>(up)) {
            return up;
        }
        while (up != sentinel() && item == deref_link<Right>(up)) {
            item = up;
            up = deref_link<Up>(up);
        }
        return up;
    }

    template<std::size_t Right>
    const_link_type get_nearest_neighbour(const_link_type item) const {
        check_direction<Right>();
        if (item == sentinel()) {
            return sentinel();
        }
        constexpr std::size_t Up = node_type::Up;
        constexpr std::size_t Left = node_type::template swap_left_right<Right>();
        if (deref_link<Right>(item) != sentinel()) {
            auto result = get_directmost_neighbour<Left>(deref_link<Right>(item));
            return result;
        }
        const_link_type up = deref_link<Up>(item);
        if (up != sentinel() && item == deref_link<Left>(up)) {
            return up;
        }
        while (up != sentinel() && item == deref_link<Right>(up)) {
            item = up;
            up = deref_link<Up>(up);
        }
        return up;
    }

protected:
    template<std::size_t direction>
    void check_direction() const {
        static_assert(direction == node_type::Left || direction == node_type::Right
                          || direction == node_type::Up,
                      "direction must be left, right or up");
    };

    bool less(key_type k1, key_type k2) const { return comparator(k1, k2); }

    bool greater(key_type k1, key_type k2) const { return comparator(k2, k1); }

    bool equal(key_type k1, key_type k2) const { return !less(k1, k2) && !greater(k1, k2); }

    std::pair<link_type, bool> insert_at(link_type & at, key_type key) {
        link_type * cur_link = &at;
        link_type prev = sentinel();
        for (;;) {
            if (*cur_link == sentinel()) {
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
