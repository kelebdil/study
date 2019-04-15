#include <iostream>

#include "bintree.h"
#include "base_node.h"

class Node;

constexpr Node * node_sentinel = nullptr;

class Node : public detail::BaseNode<Node, int, Node*, node_sentinel>
{
};

int main(int , char ** )
{
    BinTree<int, Node> tree;
    return 0;
}
