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
    Tree tree;
    /*               8
     *       4                12
     *   2       6        10     14
     * 1   3   5   7    9   11 13   15
     */
    std::vector<std::uint64_t> numbers = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};

    for (const auto & i : numbers) {
        std::cout << "==============" << std::endl;
        tree.insert(i);
        std::cout << std::endl;
        std::cout << "==============" << std::endl;
        std::cout << tree.dump_tree();
        std::cout << "==============" << std::endl;

        for(auto it = tree.begin(); it != Node::sentinel; it = tree.get_nearest_neighbour<detail::right>(it)) {
            std::cout << std::endl << "Current it: " << it << std::endl;
        }
        std::cout << std::endl;
        std::cout << "==============" << std::endl;
    }

    Tree t2;
    std::shuffle(numbers.begin(), numbers.end(), std::mt19937{std::random_device{}()});
    for(const auto & i : numbers) {
        t2.insert(i);
    }

    std::cout << t2.dump_tree() << std::endl;

    for (auto it = t2.begin(); it != Node::sentinel; it = tree.get_nearest_neighbour<detail::right>(it)) {
        std::cout << it->key() << " ";
    }
    std::cout << std::endl;
    return 0;
}
