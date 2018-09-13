#include "include/data_structures/avl_tree.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <stack>

using db_entry = std::pair<uint32_t, std::string>;

static std::vector<db_entry> parse_db_list(char* infile)
{
  std::vector<db_entry> db_list;
  std::string name = "";
  std::string id_s = "";
  int id = 0;

  std::string line, col;
  std::ifstream file(infile);
  char* endp = NULL;
  int lnum = 0;

  if (file.is_open()) {
    lnum = 0;
    try {
      while (std::getline(file, line)) {
        std::stringstream linestream(line);
        std::vector<std::string> cols;

        while (std::getline(linestream, col, ',')) cols.push_back(col);

        if (cols.size() > 2) {
          throw std::invalid_argument(
            "Invalid number of rows " + std::to_string(cols.size())
            + " in file \"" + std::string(infile) + "\" line "
            + std::to_string(lnum)
          );
        }

        name = cols.at(0);
        id_s = cols.at(1);
        id = strtod(id_s.c_str(), &endp);

        if (endp != id_s.c_str()+id_s.size()) {
          throw std::invalid_argument(
            "Invalid id \"" + id_s + "\" in file \"" + std::string(infile)
            + "\" line " + std::to_string(lnum)
          );
        }

        db_list.push_back(std::make_pair(id, name));
        lnum++;
      }
    }
    catch (const std::exception& e) {
      file.close();
      std::cerr << e.what() << std::endl << std::endl;
      throw e;
    }

    file.close();
  }

  return db_list;
}

void avl_tree_create(char* infile, AVLNode** root)
{
  if (*root != NULL) {
    throw std::invalid_argument("Given AVL tree root pointer is not NULL");
  }

  for (auto& db_entry : parse_db_list(infile))
    avl_tree_insert(root, db_entry.first, db_entry.second);
}

void avl_tree_destroy(AVLNode** root)
{
  if (*root == NULL) {
    throw std::invalid_argument("Given AVL tree root pointer is NULL");
  }
  AVLNode* lchild = (*root)->lchild;
  AVLNode* rchild = (*root)->rchild;

  if (lchild != NULL) avl_tree_destroy(&lchild);
  if (rchild != NULL) avl_tree_destroy(&rchild);

  delete *root;
  *root = NULL;
}

static AVLNode* avl_tree_rotate_rr (AVLNode* z, AVLNode* y, AVLNode* x)
{
  if (z == z->parent->rchild) {
    z->parent->rchild = y;
  } else {
    z->parent->lchild = y;
  }

  y->parent = z->parent;
  z->parent = y;

  z->rchild = y->lchild;
  z->rheight = y->lheight;

  y->lchild = z;
  y->lheight = avl_tree_get_max_height(z);

  return y;
}

static AVLNode* avl_tree_rotate_ll (AVLNode* z, AVLNode* y, AVLNode* x)
{
  if (z == z->parent->rchild) {
    z->parent->rchild = y;
  } else {
    z->parent->lchild = y;
  }

  y->parent = z->parent;
  z->parent = y;

  z->lchild = y->rchild;
  z->lheight = y->rheight;

  y->rchild = z;
  y->rheight = avl_tree_get_max_height(z);
  
  return y;
}

static AVLNode* avl_tree_rotate_rl (AVLNode* z, AVLNode* y, AVLNode* x)
{
  x->parent = z;
  y->parent = x;

  y->lchild = x->rchild;
  y->lheight = x->rheight;

  x->rchild = y;
  x->rheight = avl_tree_get_max_height(y);

  z->rchild = x;
  z->rheight = avl_tree_get_max_height(x);

  return avl_tree_rotate_rr(z, x, y);
}

static AVLNode* avl_tree_rotate_lr (AVLNode* z, AVLNode* y, AVLNode* x)
{
  x->parent = z;
  y->parent = x;

  y->rchild = x->lchild;
  y->rheight = x->lheight;

  x->lchild = y;
  x->lheight = avl_tree_get_max_height(y);

  z->lchild = x;
  z->lheight = avl_tree_get_max_height(x);

  return avl_tree_rotate_lr(z, x, y);
}

void avl_tree_insert(AVLNode** root, uint32_t id, std::string name)
{
  AVLNode* current = NULL;
  AVLNode** next = root;
  bool is_right;

  // Regular BST insertion
  while (*next != NULL) {
    current = *next;
    is_right = (id > current->id);
    next = is_right ? &(current->rchild) : &(current->lchild);
  }

  *next = new AVLNode;
  (*next)->parent = current;
  (*next)->id = id;
  (*next)->name = name;

  // Traverse tree upwards from inserted leaf
  std::stack<std::pair<AVLNode*, bool>> nodes_info;
  std::pair<AVLNode*, bool> y_info;
  std::pair<AVLNode*, bool> x_info;
  bool is_unbalanced;

  current = *next;
  is_unbalanced = (std::abs(avl_tree_get_balance_factor(current)) >= 2);

  while (current != *root && !is_unbalanced) {
    is_right = (current == current->parent->rchild);
    nodes_info.push(std::make_pair(current, is_right));

    if (is_right) {
      current->parent->rheight = avl_tree_get_max_height(current);
    } else {
      current->parent->lheight = avl_tree_get_max_height(current);
    }

    current = current->parent;
    is_unbalanced = (std::abs(avl_tree_get_balance_factor(current)) >= 2);
  }

  if (is_unbalanced) {
    y_info = nodes_info.top();
    nodes_info.pop();
    x_info = nodes_info.top();
    nodes_info.pop();

    // RR
    if (y_info.second && x_info.second) {
      current = avl_tree_rotate_rr(current, y_info.first, x_info.first);
    }
    // RL
    else if (y_info.second && !x_info.second) {
      current = avl_tree_rotate_rl(current, y_info.first, x_info.first);
    }
    // LR
    else if (!y_info.second && x_info.second) {
      current = avl_tree_rotate_rl(current, y_info.first, x_info.first);
    }
    // LL
    else if (!y_info.second && !x_info.second) {
      current = avl_tree_rotate_ll(current, y_info.first, x_info.first);
    }
  }

  // Update heights for path up to the root
  int height = -1;
  int n_height = 0;

  while (current != *root && n_height != height) {
    is_right = (current == current->parent->rchild);
    current = current->parent;
    height = avl_tree_get_max_height(current);

    if (is_right) {
      current->rheight = avl_tree_get_max_height(current->rchild);
    } else {
      current->lheight = avl_tree_get_max_height(current->lchild);
    }

    n_height = avl_tree_get_max_height(current);
  }

}

void avl_tree_remove(AVLNode** root, uint32_t id)
{
  
}

int avl_tree_get_size(AVLNode* root)
{
  if (root == NULL) {
    throw std::invalid_argument("Given AVL tree root pointer is NULL");
  }

  AVLNode* lchild = root->lchild;
  AVLNode* rchild = root->rchild;

  int lsize = (lchild != NULL) ? avl_tree_get_size(lchild) : 0;
  int rsize = (rchild != NULL) ? avl_tree_get_size(rchild) : 0;

  return 1 + lsize + rsize;
}

int avl_tree_get_balance_factor(AVLNode* root)
{
  if (root == NULL) {
    throw std::invalid_argument("Given AVL tree root pointer is NULL");
  }
  return root->rheight - root->lheight;
}

int avl_tree_get_max_height(AVLNode* root)
{
  if (root == NULL) {
    throw std::invalid_argument("Given AVL tree root pointer is NULL");
  }
  return 1 + std::max(root->lheight, root->rheight);
}
