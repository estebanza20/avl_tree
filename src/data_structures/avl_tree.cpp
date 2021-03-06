#include "include/data_structures/avl_tree.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <stack>
#include <queue>

//! Reads the input file and returns a vector of DB entries (id-name pairs)
static int parse_db_list(std::string infile, std::vector<db_entry>* db_list)
{
  std::string name = "";
  std::string id_s = "";
  int id = 0;

  std::string line, col;
  std::ifstream file(infile);
  char* endp = NULL;
  int lnum = 0;

  if (file.is_open()) {
    lnum = 0;
    while (std::getline(file, line)) {
      std::stringstream linestream(line);
      std::vector<std::string> cols;

      while (std::getline(linestream, col, ',')) cols.push_back(col);

      if (cols.size() > 2) {
        std::cerr << "Invalid number of rows " << std::to_string(cols.size())
                  << " in file \"" << std::string(infile) << "\" line "
                  << std::to_string(lnum) << std::endl;
        file.close();
        return INVALID_FILE;
      }

      name = cols.at(0);
      id_s = cols.at(1);
      id = strtod(id_s.c_str(), &endp);

      if (endp != id_s.c_str()+id_s.size()) {
        std::cerr << "Invalid id \"" << id_s << "\" in file \""
                  << std::string(infile) << "\" line "
                  << std::to_string(lnum) << std::endl;
        file.close();
        return INVALID_KEY;
      }

      db_list->push_back(std::make_pair(id, name));
      lnum++;
    }

    file.close();
  }

  return RET_OK;
}

int avl_tree_create(std::string infile, AVLNode** root)
{
  int ret = RET_OK;
  std::vector<db_entry> db_list;

  if (*root != NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is not NULL" << std::endl;
    return INVALID_TREE;
  }

  ret = parse_db_list(infile, &db_list);
  if (ret) return ret;

  // Insert each parsed DB entry into the tree
  for (auto& db_entry : db_list) {
    ret = avl_tree_insert(root, db_entry.first, db_entry.second);
    if (ret && (ret != INVALID_KEY) && (ret != KEY_EXISTS)) return ret;
  }

  return ret;
}

int avl_tree_destroy(AVLNode** root)
{
  AVLNode* lchild = NULL;
  AVLNode* rchild = NULL;

  if (*root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  lchild = (*root)->lchild;
  rchild = (*root)->rchild;

  if (lchild != NULL) avl_tree_destroy(&lchild);
  if (rchild != NULL) avl_tree_destroy(&rchild);

  delete *root;
  *root = NULL;

  return RET_OK;
}

// Perform AVL Tree RR rotation
static AVLNode* avl_tree_rotate_rr (AVLNode** root, AVLNode* z, AVLNode* y)
{
  if (z->parent) {
    if (z == z->parent->rchild) {
      z->parent->rchild = y;
    } else {
      z->parent->lchild = y;
    }
  } else {
    // y is the new root
    *root = y;
  }

  y->parent = z->parent;

  z->rchild = y->lchild;
  z->rheight = y->lheight;
  if (y->lchild) y->lchild->parent = z;

  y->lchild = z;
  avl_tree_get_max_height(z, &(y->lheight));
  z->parent = y;

  return y;
}

// Perform AVL Tree LL rotation
static AVLNode* avl_tree_rotate_ll (AVLNode** root, AVLNode* z, AVLNode* y)
{
  if (z->parent) {
    if (z == z->parent->rchild) {
      z->parent->rchild = y;
    } else {
      z->parent->lchild = y;
    }
  } else {
    // y is the new root
    *root = y;
  }

  y->parent = z->parent;

  z->lchild = y->rchild;
  z->lheight = y->rheight;
  if (y->rchild) y->rchild->parent = z;

  y->rchild = z;
  avl_tree_get_max_height(z, &(y->rheight));
  z->parent = y;
  
  return y;
}

// Perform AVL Tree RL rotation
static AVLNode* avl_tree_rotate_rl (AVLNode** root, AVLNode* z, AVLNode* y, AVLNode* x)
{
  y->lchild = x->rchild;
  y->lheight = x->rheight;
  if (x->rchild) x->rchild->parent = y;

  x->rchild = y;
  avl_tree_get_max_height(y, &(x->rheight));
  y->parent = x;

  z->rchild = x;
  avl_tree_get_max_height(x, &(z->rheight));
  x->parent = z;

  return avl_tree_rotate_rr(root, z, x);
}

// Perform AVL Tree LR rotation
static AVLNode* avl_tree_rotate_lr (AVLNode** root, AVLNode* z, AVLNode* y, AVLNode* x)
{
  y->rchild = x->lchild;
  y->rheight = x->lheight;
  if (x->lchild) x->lchild->parent = y;

  x->lchild = y;
  avl_tree_get_max_height(y, &(x->lheight));
  y->parent = x;

  z->lchild = x;
  avl_tree_get_max_height(x, &(z->lheight));
  x->parent = z;

  return avl_tree_rotate_ll(root, z, x);
}

//! 
/* Rebalance the AVL Tree. Updates heights and calculates balance factors,
 * if an unbalanced node is detected apply rotation.
 */
static int avl_tree_rebalance(AVLNode** root, AVLNode* node)
{
  std::stack<std::pair<AVLNode*, bool>> nodes_info;

  std::pair<AVLNode*, bool> y_info;
  std::pair<AVLNode*, bool> x_info;

  int balance_factor = 0;
  bool is_unbalanced = false;
  bool is_right = false;

  int height = -1;
  int n_height = 0;

  // Traverse tree upwards from inserted leaf
  avl_tree_get_balance_factor(node, &balance_factor);
  is_unbalanced = (std::abs(balance_factor) >= 2);

  while (node != *root && !is_unbalanced) {
    is_right = (node == node->parent->rchild);
    nodes_info.push(std::make_pair(node, is_right));

    if (is_right) {
      avl_tree_get_max_height(node, &(node->parent->rheight));
    } else {
      avl_tree_get_max_height(node, &(node->parent->lheight));
    }

    node = node->parent;

    avl_tree_get_balance_factor(node, &balance_factor);
    is_unbalanced = (std::abs(balance_factor) >= 2);
  }

  // If unbalanced, apply rotation
  if (is_unbalanced) {
    y_info = nodes_info.top();
    nodes_info.pop();
    x_info = nodes_info.top();
    nodes_info.pop();

    // RR
    if (y_info.second && x_info.second) {
      node = avl_tree_rotate_rr(root, node, y_info.first);
    }
    // RL
    else if (y_info.second && !x_info.second) {
      node = avl_tree_rotate_rl(root, node, y_info.first, x_info.first);
    }
    // LR
    else if (!y_info.second && x_info.second) {
      node = avl_tree_rotate_lr(root, node, y_info.first, x_info.first);
    }
    // LL
    else if (!y_info.second && !x_info.second) {
      node = avl_tree_rotate_ll(root, node, y_info.first);
    }
  }

  // Update heights for path up to the root
  while (node != *root && n_height != height) {
    is_right = (node == node->parent->rchild);
    node = node->parent;
    avl_tree_get_max_height(node, &height);

    if (is_right) {
      avl_tree_get_max_height(node->rchild, &(node->rheight));
    } else {
      avl_tree_get_max_height(node->lchild, &(node->lheight));
    }

    avl_tree_get_max_height(node, &n_height);
  }

  return RET_OK;
}

int avl_tree_insert(AVLNode** root, uint32_t id, std::string name)
{
  AVLNode* current = NULL;
  bool is_right = false;
  bool found = false;

  if (*root) {
    // Search of node insertion position
    avl_tree_search(*root, id, &current, &found);
    if (found) {
      std::cerr << "Invalid insertion: Key already exists" << std::endl;
      return KEY_EXISTS;
    }
  } else {
    current = NULL;
  }

  if ((id < MIN_ID) || (id > MAX_ID)) {
    std::cerr << "Invalid key (" << id << "): Value out of range" << std::endl;
    return INVALID_KEY;
  }

  // Insert new node in proper position
  if (!current) {
    current = new AVLNode;
    current->parent = NULL;
    current->id = id;
    current->name = name;
    *root = current;
  } else {
    is_right = (id > current->id);
    if (is_right) {
      current->rchild = new AVLNode;
      current->rchild->parent = current;
      current->rchild->id = id;
      current->rchild->name = name;
      current = current->rchild;
    } else {
      current->lchild = new AVLNode;
      current->lchild->parent = current;
      current->lchild->id = id;
      current->lchild->name = name;
      current = current->lchild;
    }
  }

  // Rebalance the tree after insertion
  avl_tree_rebalance(root, current);

  return RET_OK;
}

int avl_tree_search(AVLNode* root, uint32_t id, AVLNode** node, bool* found)
{
  AVLNode* next;
  bool is_right;

  *found = false;

  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  *node = next = root;

  // BST traversal, compare ID to detect if node is found
  while (next != NULL && !*found) {
    *node = next;
    is_right = (id > (*node)->id);
    next = is_right ? (*node)->rchild : (*node)->lchild;
    *found = ((*node)->id == id);
  }

  return RET_OK;
}

int avl_tree_get_max_node(AVLNode* root, AVLNode** node)
{
  AVLNode* next;

  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  *node = next = root;

  while (next != NULL) {
    *node = next;
    next = (*node)->rchild;
  }

  return RET_OK;
}

int avl_tree_get_min_node(AVLNode* root, AVLNode** node)
{
  AVLNode* next;

  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  *node = next = root;

  while (next != NULL) {
    *node = next;
    next = (*node)->lchild;
  }

  return RET_OK;
}

// FIXME: Implement rebalance step
//! Removes a node from the AVL Tree
int avl_tree_remove(AVLNode** root, uint32_t id)
{
  AVLNode* current = NULL;
  AVLNode* replace = NULL;
  AVLNode* del_node = NULL;
  bool is_right = false;
  bool found = false;

  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  avl_tree_search(*root, id, &current, &found);
  if (!found) {
    std::cerr << "Invalid deletion: Key not found" << std::endl;
    return KEY_NOT_FOUND;
  }

  if (!current->rchild && !current->lchild) {
    if (current->parent) {
      is_right = (current == current->parent->rchild);
      if (is_right) {
        current->parent->rchild = NULL;
        current->parent->rheight = 0;
      } else {
        current->parent->lchild = NULL;
        current->parent->lheight = 0;
      }
    }
    del_node = current;

  } else if (current->rchild && !current->lchild) {
    if (current->parent) {
      is_right = (current == current->parent->rchild);
      if (is_right) {
        current->parent->rchild = current->rchild;
        current->parent->rheight = current->rheight;
      } else {
        current->parent->lchild = current->rchild;
        current->parent->lheight = current->rheight;
      }
    }
    current->rchild->parent = current->parent;
    del_node = current;

  } else if (!current->rchild && current->lchild) {
    if (current->parent) {
      is_right = (current == current->parent->rchild);
      if (is_right) {
        current->parent->rchild = current->lchild;
        current->parent->rheight = current->lheight;
      } else {
        current->parent->lchild = current->lchild;
        current->parent->lheight = current->lheight;
      }
    }
    current->lchild->parent = current->parent;
    del_node = current;

  } else if (current->rchild && current->lchild) {
    // Look for minimum key node on right subtree
    avl_tree_get_min_node(current->rchild, &replace);

    if (replace->rchild) replace->rchild->parent = replace->parent;

    is_right = (replace == replace->parent->rchild);
    if (is_right) {
      replace->parent->rchild = replace->rchild;
      replace->parent->rheight = replace->rheight;
    } else {
      replace->parent->lchild = replace->rchild;
      replace->parent->lheight = replace->rheight;
    }

    current->id = replace->id;
    current->name = replace->name;

    del_node = replace;
  }

  if (del_node == *root) *root = NULL;
  delete del_node;

  return RET_OK;
}

int avl_tree_get_size(AVLNode* root, int* size)
{
  AVLNode* lchild = NULL;
  AVLNode* rchild = NULL;
  int lsize = 0;
  int rsize = 0;

  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  lchild = root->lchild;
  rchild = root->rchild;

  if (lchild != NULL) avl_tree_get_size(lchild, &lsize);
  if (rchild != NULL) avl_tree_get_size(rchild, &rsize);

  *size = 1 + lsize + rsize;
  return RET_OK;
}

int avl_tree_get_balance_factor(AVLNode* root, int* balance_factor)
{
  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  *balance_factor = root->rheight - root->lheight;
  return RET_OK;
}

int avl_tree_get_max_height(AVLNode* root, int* max_height)
{
  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  *max_height = 1 + std::max(root->lheight, root->rheight);
  return RET_OK;
}

int avl_tree_print(AVLNode* root)
{
  if (root == NULL) {
    std::cerr << "Invalid tree: Given AVL tree root pointer is NULL" << std::endl;
    return INVALID_TREE;
  }

  int level = 0;
  uint32_t id = 0;
  uint32_t parent_id = 0;
  uint32_t lchild_id = 0;
  uint32_t rchild_id = 0;
  std::string name = "";
  std::string parent_name = "";
  std::string lchild_name = "";
  std::string rchild_name = "";

  std::queue<AVLNode*> barrier;
  AVLNode* it = NULL;

  barrier.push(root);

  std::cout << "======================================================" << std::endl;
  std::cout << "-------------------  AVL Tree  -----------------------" << std::endl;
  std::cout << "======================================================" << std::endl;

  // BFS traversal of the tree
  while(!barrier.empty()) {
    level = 0;
    id = 0;
    parent_id = 0;
    lchild_id = 0;
    rchild_id = 0;
    name = "";
    parent_name = "";
    lchild_name = "";
    rchild_name = "";

    it = barrier.front();
    barrier.pop();

    // On each node prints parent, child and current info
    id = it->id;
    name = it->name;

    if (it->parent) {
      parent_id = it->parent->id;
      parent_name = it->parent->name;
    }

    if (it->lchild) {
      lchild_id = it->lchild->id;
      lchild_name = it->lchild->name;
      barrier.push(it->lchild);
    }

    if (it->rchild) {
      rchild_id = it->rchild->id;
      rchild_name = it->rchild->name;
      barrier.push(it->rchild);
    }

    level = 0;
    while(it != root) { it = it->parent; level++; }

    std::cout << "==================================================" << std::endl;
    std::cout << "(node id): " << id << std::endl;
    std::cout << "(node name): " << name << std::endl;
    std::cout << "(node level): " << level << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "(parent id): " << parent_id << std::endl;
    std::cout << "(parent name): " << parent_name << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "(lchild id): " << lchild_id
              << " (rchild id): " << rchild_id << std::endl;
    std::cout << "(lchild name): " << lchild_name
              << " (rchild name): " << rchild_name <<std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
  }
  std::cout << "======================================================"
            << std::endl << std::endl;

  return RET_OK;
}

