#include <iostream>

#include "bintree.h"

#include <cassert>
#include <cstdint>
#include <random>
#include <vector>

using Tree = BinTree<std::uint64_t>;
using Node = Tree::node_type;

void print_node(Node const & node) {
    std::cout << node.key() << " ";
}

int main(int, char **) {
    std::vector<std::uint64_t> numbers = { 8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15 };
    std::vector<std::uint64_t> sorted_numbers = numbers;
    std::sort(sorted_numbers.begin(), sorted_numbers.end());

    Tree tree;

    for (const auto & i : numbers) {
        tree.insert(i);
    }

    std::size_t cursor = 0;
    for (Tree::iterator it = tree.begin(); it != tree.end(); ++it) {
        assert(*it == sorted_numbers[cursor++]);
    }

    cursor = sorted_numbers.size() - 1;
    for (Tree::reverese_iterator it = tree.rbegin(); it != tree.rend(); ++it) {
        assert(*it == sorted_numbers[cursor--]);
    }

    Tree t2;
    std::shuffle(numbers.begin(), numbers.end(), std::mt19937{ std::random_device{}() });
    for (const auto & i : numbers) {
        t2.insert(i);
    }

    cursor = 0;
    for (Tree::const_iterator it = t2.begin(); it != t2.end(); ++it) {
        assert(*it == sorted_numbers[cursor++]);
    }

    cursor = sorted_numbers.size() - 1;
    for (Tree::const_reverese_iterator it = t2.rbegin(); it != t2.rend(); ++it) {
        assert(*it == sorted_numbers[cursor--]);
    }

    return 0;
}
