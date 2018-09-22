#include "include/data_structures/avl_tree.hpp"
#include <iostream>

int main(void) {
  AVLNode* avl_tree = NULL;
  char file[] = "misc/input/name_id_list.txt";
  int size = 0;
  int height = 0;

  avl_tree_create(file, &avl_tree);

  avl_tree_insert(&avl_tree, 103426764, "Son Goku");

  avl_tree_get_size(avl_tree, &size);
  std::cout << "AVL tree size: " << size << std::endl;

  avl_tree_get_max_height(avl_tree, &height);
  std::cout << "AVL tree max_height: " << height << std::endl;

  avl_tree_remove(&avl_tree, 123456789);

  avl_tree_get_size(avl_tree, &size);
  std::cout << "AVL tree size: " << size << std::endl;

  avl_tree_destroy(&avl_tree);

  return 0;
}
