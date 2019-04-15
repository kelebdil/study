#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>


namespace detail {

static constexpr std::size_t links_size = 3;
static constexpr std::size_t left = 0;
static constexpr std::size_t right = 1;
static constexpr std::size_t up = 2;

template <
    typename ParentType,
    typename KeyType,
    typename LinkType,
    LinkType Sentinel
    >
class BaseNode;

template <
    typename ParentType,
    typename KeyType,
    typename LinkType,
    LinkType Sentinel = nullptr
    >
class BaseNode
{
public:
    typedef LinkType link_type;
    typedef KeyType key_type;
    typedef ParentType parent_type;

    template <
        typename P1,
        typename K1,
        typename L1,
        L1 NewSentinel
    > struct rebind {
        typedef BaseNode<P1, K1, L1, NewSentinel> other;
    };

    typedef std::array<link_type, detail::links_size> links_array;

    static constexpr link_type sentinel = Sentinel;

    BaseNode(key_type key = key_type()) :
        key_(std::move(key))
    {
        fill(links.begin(), links.end(), sentinel);
    }

    const links_array & links() const {
        return links_;
    }

    links_array & links() {
        return links_;
    }

    link_type & left() {
        return links_[detail::left];
    }

    link_type left() const {
        return links_[detail::left];
    }

    link_type & right() {
        return links_[detail::right];
    }

    link_type rigth() const {
        return links_[detail::right];
    }

    link_type & up() {
        return links_[detail::up];
    }

    link_type up() const {
        return links_[detail::up];
    }

    key_type & key() {
        return key_;
    }

    const key_type & key() const {
        return key_;
    }

private:
    links_array links_;
    key_type key_;
};


std::size_t swap_left_right(std::size_t left_or_right) {
    if (left_or_right == detail::left) {
        return detail::right;
    } else if (left_or_right == detail::right) {
        return detail::left;
    } else if (left_or_right == detail::up) {
        return detail::up;
    } else {
        throw std::logic_error("swap_left_right");
    }
}

} // namespace detail
