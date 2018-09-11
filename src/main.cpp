#include "include/data_structures/avl_tree.hpp"
#include <iostream>

int main(void) {
  Node* avl_tree;
  char* file = "list.txt";
  int size = 0;
  int height = 0;

  avl_tree_create(file, &avl_tree);

  avl_tree_insert(avl_tree, 12345678, "Fulanito");

  size = avl_tree_get_size(avl_tree);
  std::cout << "AVL tree size: " << size;

  size = avl_tree_get_max_height(avl_tree);
  std::cout << "AVL tree max_height: " << height;

  avl_tree_delete(avl_tree, 12345678);

  avl_tree_destroy(&avl_tree);

  return 0;
}
