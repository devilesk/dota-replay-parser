#include "huffman.hpp"

bool compare_tree(HuffmanTree i, HuffmanTree j) {
  if (i.weight == j.weight) {
    return i.value < j.value;
  }
  else {
    return i.weight >= j.weight;
  }
}

HuffmanTree buildTree(int symFreqs[], int size) {
  //std::cout << "buildTree\n";
  std::vector<HuffmanTree> trees;
  for (int i = 0; i < size; ++i) {
    if (symFreqs[i] == 0) {
      symFreqs[i] = 1;
    }
    //std::cout << "i: " << std::to_string(i) << "\n";
    //std::cout << "symFreqs: " << std::to_string(symFreqs[i]) << "\n";
    HuffmanTree new_leaf = {
      symFreqs[i],
      true,
      i,
      NULL,
      NULL
    };
    trees.push_back(std::move(new_leaf));
  }
  
  int n = size;
  //std::cout << "buildTree while start\n";
  //std::cout << "tree size " << std::to_string(trees.size()) << "\n";
  std::make_heap(trees.begin(), trees.end(), compare_tree);
  //std::cout << "heap made\n";
  //std::cout << "heap size " << std::to_string(trees.size()) << "\n";
  while (trees.size() > 1) {
    //std::cout << "while start " << std::to_string(trees.size()) << "\n";
    std::pop_heap(trees.begin(), trees.end(), compare_tree);
    HuffmanTree* a = new HuffmanTree();
    *a = trees.back();
    trees.pop_back();
    std::pop_heap(trees.begin(), trees.end(), compare_tree);
    HuffmanTree* b = new HuffmanTree();
    *b = trees.back();
    trees.pop_back();
    //std::cout << "while after pop " << std::to_string(trees.size()) << "\n";
    //std::cout << "a weight: " << std::to_string(a->weight) << "\n";
    //std::cout << "b weight: " << std::to_string(b->weight) << "\n";
    //std::cout << "new_node weight: " << std::to_string(a->weight + b->weight) << "\n";
    HuffmanTree new_node = {
      a->weight + b->weight,
      false,
      n,
      a,
      b
    };
    trees.push_back(std::move(new_node));
    std::push_heap(trees.begin(), trees.end(), compare_tree);
    //std::cout << "while after push " << std::to_string(trees.size()) << "\n";
    ++n;
    //std::cout << "n: " << std::to_string(n) << "\n";
  }
  //std::cout << "while end\n";
  std::pop_heap(trees.begin(), trees.end(), compare_tree);
  //std::cout << "end pop heap\n";
  HuffmanTree ret = trees.back();
  trees.pop_back();
  //std::cout << "buildTree end\n";
  return ret;
}

void print_node(HuffmanTree* node) {
  //std::cout << "weight: " << std::to_string(node->weight) << "\n";
  //std::cout << "isLeaf: " << std::to_string(node->isLeaf) << "\n";
  //std::cout << "value: " << std::to_string(node->value) << "\n";
}