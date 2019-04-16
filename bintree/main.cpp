#include <iostream>

#include "bintree.h"
#include <vector>
#include <cassert>
#include <random>
#include <cstdint>

typedef BinTree<std::uint64_t> Tree;
typedef Tree::node_type Node;

void print_node(const Node &node) {
    std::cout << node.key() << " ";
}

int main(int , char ** )
{
    std::vector<std::uint64_t> numbers = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
    std::vector<std::uint64_t> sorted_numbers = numbers;
    std::sort(sorted_numbers.begin(), sorted_numbers.end());

    Tree tree;

    for (const auto & i : numbers) {
        tree.insert(i);
    }

    std::size_t cursor = 0;
    for(Tree::iterator it = tree.begin(); it != tree.end(); ++it) {
        assert(*it == sorted_numbers[cursor++]);
    }

    Tree t2;
    std::shuffle(numbers.begin(), numbers.end(), std::mt19937{std::random_device{}()});
    for(const auto & i : numbers) {
        t2.insert(i);
    }

    cursor = 0;
    for(Tree::const_iterator it = t2.begin(); it != t2.end(); ++it) {
        assert(*it == sorted_numbers[cursor++]);
    }

    return 0;
}
