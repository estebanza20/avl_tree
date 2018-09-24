#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <sys/time.h>

#include "include/data_structures/avl_tree.hpp"

/**
 * Utilitary function to recursively calculate tree max height
 * (not relying on AVL Tree lheight and rheight metadata).
 **/
int calc_tree_max_height(AVLNode* root) {
  int lheight = 0;
  int rheight = 0;

  if (root->lchild) lheight = calc_tree_max_height(root->lchild);
  if (root->rchild) rheight = calc_tree_max_height(root->rchild);

  return 1 + std::max(lheight, rheight);
}

/**
 * Validates AVL Tree properties (BST and balance factor)
 * on each node in the tree.
 **/
static void validate_avl_tree(AVLNode* root) {
  int lheight = 0;
  int rheight = 0;

  if (root->lchild) {
    lheight = calc_tree_max_height(root->lchild);
    ASSERT_LE(root->lchild->id, root->id);
    ASSERT_EQ(root->lchild->parent, root);
    validate_avl_tree(root->lchild);
  }

  if (root->rchild) {
    rheight = calc_tree_max_height(root->rchild);
    ASSERT_GT(root->rchild->id, root->id);
    ASSERT_EQ(root->rchild->parent, root);
    validate_avl_tree(root->rchild);
  }

  ASSERT_LT(std::abs(rheight-lheight), 2);
}

// Test AVL tree create from file and destroy operations
TEST(AVLTreeTest, CreateDestroy) {
  int ret = 0;
  AVLNode* avl_tree = NULL;
  char file[] = "misc/input/lista_10.txt";

  ret = avl_tree_create(file, &avl_tree);
  ASSERT_EQ(ret, RET_OK);
  ASSERT_NE(avl_tree, nullptr);

  ret = avl_tree_create(file, &avl_tree);
  ASSERT_EQ(ret, INVALID_TREE);

  ret = avl_tree_destroy(&avl_tree);
  ASSERT_EQ(ret, RET_OK);
  ASSERT_EQ(avl_tree, nullptr);

  ret = avl_tree_destroy(&avl_tree);
  ASSERT_EQ(ret, INVALID_TREE);
}

// Test valid and invalid AVL Tree predefined insertions
TEST(AVLTreeTest, InsertNodesBasic) {
  int ret = 0;
  int size = 0;
  AVLNode* avl_tree = NULL;

  ret = avl_tree_insert(&avl_tree, 101010101, "Daft Punk");
  ASSERT_EQ(ret, RET_OK);
  ASSERT_NE(avl_tree, nullptr);
  
  std::vector<db_entry> db_entries = {
    {121212121, "Ash Ketchum"},
    {897651234, "Gary Oak"},
    {112345678, "Jack Jack"},
    {100789765, "Richard Stallman"},
    {203123312, "Son Goku"},
    {921410571, "Linus Torvals"},
    {622913547, "Foo Bar"},
    {521010534, "Dennis Ritchie"},
    {421017514, "Ma Lin"},
    {434657890, "Johnny Bravo"}
  };

  for (db_entry& entry : db_entries) {
    ret = avl_tree_insert(&avl_tree, entry.first, entry.second);
    ASSERT_EQ(ret, RET_OK);
  }

  ret = avl_tree_get_size(avl_tree, &size);
  ASSERT_EQ(ret, RET_OK);
  ASSERT_EQ(size, 11);

  avl_tree_print(avl_tree);

  validate_avl_tree(avl_tree);

  ret = avl_tree_insert(&avl_tree, 921410571, "Gohan");
  ASSERT_EQ(ret, KEY_EXISTS);

  ret = avl_tree_insert(&avl_tree, 1100000000, "Majin Boo");
  ASSERT_EQ(ret, INVALID_KEY);

  ret = avl_tree_insert(&avl_tree, 500, "Babidi");
  ASSERT_EQ(ret, INVALID_KEY);
}

// Test random node insertions, validating the AVL Tree after each insertion
TEST(AVLTreeTest, InsertNodesValidate) {
  int ret = 0;
  int size = 0;
  int exp_size = 0;
  bool found = false;

  AVLNode* avl_tree = NULL;
  AVLNode* avl_node = NULL;
  const int num_inserts = 1000;

  for (int i = 0; i < num_inserts; i++) {
    uint32_t id = MIN_ID + rand() % (MAX_ID-MIN_ID);

    ret = avl_tree_search(avl_tree, id, &avl_node, &found);
    if (avl_tree) {
      ASSERT_EQ(ret, RET_OK);
    } else {
      ASSERT_EQ(ret, INVALID_TREE);
    }

    if (!found) {
      ret = avl_tree_insert(&avl_tree, id, "");
      exp_size++;
    }

    validate_avl_tree(avl_tree);
  }

  ret = avl_tree_get_size(avl_tree, &size);
  ASSERT_EQ(ret, RET_OK);
  ASSERT_EQ(size, exp_size);
}

/**
* Test the creation of multiple AVL Trees, by inserting an incrementally
* large number of nodes along multiple iterations, validating the tree after
* each set of insertions (iteration). Also prints the execution time of
* the tree creation.
**/

TEST(AVLTreeTest, InsertNodesStress) {
  int size = 0;
  int max_height = 0;
  AVLNode* avl_tree = NULL;
  const int num_iterations = 10;
  const int step_size = 10000;
  uint32_t id = 0;

  for (int i = 1; i < num_iterations; i++) {
    std::chrono::microseconds time;
    std::chrono::high_resolution_clock::time_point start, finish;

    start = std::chrono::high_resolution_clock::now();

    for (int j = 0; j < i * step_size; j++) {
      id = MIN_ID + rand() % (MAX_ID-MIN_ID);
      avl_tree_insert(&avl_tree, id, "");
    }

    finish = std::chrono::high_resolution_clock::now();

    time = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

    avl_tree_get_size(avl_tree, &size);
    avl_tree_get_max_height(avl_tree, &max_height);

    std::cout << "size: " << size
              << " height: " << max_height
              << " time (us): " << time.count()
              << std::endl;

    validate_avl_tree(avl_tree);
    avl_tree_destroy(&avl_tree);
  }
}

int main(int argc, char **argv) {
  srand(time(0));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
