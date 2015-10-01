#ifndef _HUFFMAN_HPP_
#define _HUFFMAN_HPP_

#include <iostream>
#include <algorithm>

#include "parser_types.hpp"

extern HuffmanTree huf;

bool compare_tree(HuffmanTree i, HuffmanTree j);

HuffmanTree buildTree(int symFreqs[], int size);

void print_node(HuffmanTree* node);

#endif /* _HUFFMAN_HPP_ */