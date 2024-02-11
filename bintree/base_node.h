#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace detail
{

template<typename ParentType, typename KeyType, typename LinkType, LinkType Sentinel>
class BaseNode;

template<typename ParentType, typename KeyType, typename LinkType, LinkType Sentinel = nullptr>
class BaseNode
{
public:
    static constexpr std::size_t links_size = 3;
    enum LinkIndex {
        Left = 0,
        Right = 1,
        Up = 2,
    };

    using link_type = LinkType;
    using key_type = KeyType;
    using parent_type = ParentType;
    using links_array = std::array<link_type, links_size>;

    static constexpr link_type sentinel = Sentinel;

    BaseNode(key_type key = key_type())
        : key_{ std::move(key) } {
        std::fill(links_.begin(), links_.end(), sentinel);
    }

    virtual ~BaseNode() = default;

    const links_array & links() const { return links_; }

    links_array & links() { return links_; }

    link_type & left() { return links_[LinkIndex::Left]; }

    link_type left() const { return links_[LinkIndex::Left]; }

    link_type & right() { return links_[LinkIndex::Right]; }

    link_type rigth() const { return links_[LinkIndex::Right]; }

    link_type & up() { return links_[LinkIndex::Up]; }

    link_type up() const { return links_[LinkIndex::Up]; }

    key_type & key() { return key_; }

    const key_type & key() const { return key_; }

    template<std::size_t left_or_right>
    static constexpr std::size_t swap_left_right() {
        if (left_or_right == LinkIndex::Left) {
            return LinkIndex::Right;
        } else if (left_or_right == LinkIndex::Right) {
            return LinkIndex::Left;
        } else if (left_or_right == LinkIndex::Up) {
            return LinkIndex::Up;
        } else {
            throw std::logic_error("swap_left_right");
        }
    }
private:
    links_array links_;
    key_type key_;
};



}  // namespace detail
