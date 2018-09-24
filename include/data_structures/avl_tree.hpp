#include <iostream>
#include <string>
#include <cstdint>

#define MAX_ID 999999999
#define MIN_ID 100000000

//! Return codes of the avl_tree functions
enum {
  //! Function returns without error
  RET_OK = 0,
  //! The given tree is invalid
  INVALID_TREE = -1,
  //! The given key (id) is invalid
  INVALID_KEY = -2,
  //! The given input file is invalid
  INVALID_FILE = -4,
  //! The given key (id) was not found
  KEY_NOT_FOUND = -5,
  //! The given key (id) already exists
  KEY_EXISTS = -6
};

using db_entry = std::pair<uint32_t, std::string>;

//! AVL Tree node structure
struct AVLNode {
  //! Name of the person
  std::string name;
  //! ID of the person
  uint32_t id;

  //! Pointer to the parent node
  AVLNode* parent = NULL;
  //! Pointer to the left child node
  AVLNode* lchild = NULL;
  //! Pointer to the right child node
  AVLNode* rchild = NULL;

  //! Max height of the left subtree
  int lheight = 0;
  //! Max height of the right subtree
  int rheight = 0;
};

/**
 *  @brief Creates an AVL Tree from an input file.
 *  @param[in] infile Input file name.
 *  @param[out] root Root node of the AVL Tree to create.
 *  @return return code.
 **/
int avl_tree_create(std::string infile, AVLNode** root);

/**
 *  @brief Destroys the AVL Tree, releasing all nodes memory.
 *  @param[in,out] root Root node of the AVL Tree to destroy.
 *  @return return code.
 **/
int avl_tree_destroy(AVLNode** root);

/**
 *  @brief Insert a new node into the AVL Tree.
 *  @param[in,out] root Root node of the AVL Tree to insert the new node.
 *  @param[in] id ID of the person for the new node.
 *  @param[in] id Name of the person for the new node.
 *  @return return code.
 **/
int avl_tree_insert(AVLNode** root, uint32_t id, std::string name);
int avl_tree_remove(AVLNode** root, uint32_t id);

/**
 *  @brief Search for a node in the AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[in] id ID of the person to search for the node (key).
 *  @param[out] node Last node after tree search traversal.
 *  @param[out] found Boolean that indicates if the node was found,
 *               in which case the node parameter points to the found node.
 *  @return return code.
 **/
int avl_tree_search(AVLNode* root, uint32_t id, AVLNode** node, bool* found);

/**
 *  @brief Get the size (number of elements) in the AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[out] size Size of the AVL Tree.
 *  @return return code.
 **/
int avl_tree_get_size(AVLNode* root, int* size);

/**
 *  @brief Get the balance factor of the given AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[out] balance_factor Difference between the heights of the
 *               right and left subtree (rheight - lheight).
 *  @return return code.
 **/
int avl_tree_get_balance_factor(AVLNode* root, int* balance_factor);

/**
 *  @brief Get the max height of the given AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[out] max_height Length of the longest path from the root of
 *               the tree to a leaft node.
 *  @return return code.
 **/
int avl_tree_get_max_height(AVLNode* root, int* max_height);

/**
 *  @brief Get the maximum key (id) node of the given AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[out] node Maximum key (id) node found.
 *  @return return code.
 **/
int avl_tree_get_max_node(AVLNode* root, AVLNode** node);

/**
 *  @brief Get the minimum key (id) node of the given AVL Tree.
 *  @param[in] root Root node of the AVL Tree.
 *  @param[out] node Minimum key (id) node found.
 *  @return return code.
 **/
int avl_tree_get_min_node(AVLNode* root, AVLNode** node);

/**
 *  @brief Prints the given AVL Tree, level by level using BFS traversal.
 *  @param[in] root Root node of the AVL Tree.
 *  @return return code.
 **/
int avl_tree_print(AVLNode* root);
