#include "include/data_structures/avl_tree.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

int main(void) {
  int ret = 0;
  int size = 0;
  int max_height = 0;
  AVLNode* avl_tree = NULL;

  // Vector of input files
  std::vector<std::string> files = {
    "misc/input/lista_10.txt",
    "misc/input/lista_100.txt",
    "misc/input/lista_1000.txt",
    "misc/input/lista_5000.txt",
    "misc/input/lista_10000.txt"
  };

  std::ofstream running_times_file("misc/data/running_times.txt");

  std::chrono::microseconds time;
  std::chrono::steady_clock::time_point start, finish;

  for (auto& file : files) {
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Processing file: \"" << file << "\"" << std::endl;

    start = std::chrono::steady_clock::now();
    // Create the AVL tree from input file
    ret = avl_tree_create(file, &avl_tree);
    finish = std::chrono::steady_clock::now();

    if (ret == RET_OK) {
      time = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
      avl_tree_get_size(avl_tree, &size);
      avl_tree_get_max_height(avl_tree, &max_height);

      // Write AVL Tree creation running time (us) information in output file
      if (running_times_file.is_open()) {
        running_times_file << file << " " << size << " " << time.count() << std::endl;
      }

      // Print AVL Tree information to standard output
      std::cout << "AVL Tree creation time (us): " << time.count() << std::endl;
      std::cout << "AVL Tree size: " << size << std::endl;
      std::cout << "AVL Tree max height: " << max_height
                << std::endl << std::endl;
    } else {
      std::cout << "AVL Tree creation failed with return code " << ret
                << std::endl << std::endl;
    }

    avl_tree_destroy(&avl_tree);
  }

  if (running_times_file.is_open()) running_times_file.close();

  return 0;
}
