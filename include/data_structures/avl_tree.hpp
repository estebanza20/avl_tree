#include <iostream>
#include <string>
#include <cstdint>

struct AVLNode {
  std::string name;
  uint32_t id;

  AVLNode* parent = NULL;
  AVLNode* lchild = NULL;
  AVLNode* rchild = NULL;
  int lheight = 0;
  int rheight = 0;
};

void avl_tree_create(char* infile, AVLNode** root);
void avl_tree_destroy(AVLNode** root);
void avl_tree_insert(AVLNode** root, uint32_t id, std::string name);
void avl_tree_remove(AVLNode** root, uint32_t id);
int avl_tree_get_size(AVLNode* root);
int avl_tree_get_balance_factor(AVLNode* root);
int avl_tree_get_max_height(AVLNode* root);
