#include <iostream>
#include <string>
#include <cstdint>

#define MAX_ID 999999999
#define MIN_ID 100000000

enum {
  RET_OK = 0,
  INVALID_TREE = -1,
  INVALID_KEY = -2,
  INVALID_FILE = -4,
  KEY_NOT_FOUND = -5,
  KEY_EXISTS = -6
};


struct AVLNode {
  std::string name;
  uint32_t id;

  AVLNode* parent = NULL;
  AVLNode* lchild = NULL;
  AVLNode* rchild = NULL;
  int lheight = 0;
  int rheight = 0;
};

int avl_tree_create(char* infile, AVLNode** root);
int avl_tree_destroy(AVLNode** root);
int avl_tree_insert(AVLNode** root, uint32_t id, std::string name);
int avl_tree_remove(AVLNode** root, uint32_t id);
int avl_tree_rebalance(AVLNode* root, AVLNode* node);
int avl_tree_search(AVLNode* root, uint32_t id, AVLNode** node, bool* found);
int avl_tree_get_size(AVLNode* root, int* size);
int avl_tree_get_balance_factor(AVLNode* root, int* balance_factor);
int avl_tree_get_max_height(AVLNode* root, int* max_height);
int avl_tree_get_max_node(AVLNode* root, AVLNode** node);
int avl_tree_get_min_node(AVLNode* root, AVLNode** node);
