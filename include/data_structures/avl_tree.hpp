#include <iostream>
#include <string>
#include <cstdint>

struct Node {
  std::string name;
  uint32_t id;

  struct Node* lchild;
  struct Node* rchild;
};

void avl_tree_create(char* infile, struct Node** head);
void avl_tree_destroy(struct Node** head);
void avl_tree_insert(struct Node* head, uint32_t id, std::string name);
void avl_tree_remove(struct Node* head, uint32_t id);
int avl_tree_get_size(struct Node* head);
int avl_tree_get_max_height(struct Node* head);
