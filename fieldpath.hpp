#ifndef _FIELDPATH_HPP_
#define _FIELDPATH_HPP_

#include <string>
#include <iostream>
#include "bitstream.hpp"
#include "parser_types.hpp"
#include "huffman.hpp"

// Global fieldpath lookup array
extern std::vector<fieldpathOp> fieldpathLookup;

// Initialize a fieldpath object
fieldpath newFieldpath(dt* parentTbl, HuffmanTree* huf);

// Walk an encoded fieldpath based on a huffman tree
void walk(dota::bitstream &r, fieldpath* fp);

// Adds a field based on the current index
void addField(fieldpath* fp);

// Returns a huffman tree based on the operation weights
HuffmanTree newFieldpathHuffman();

// All different fieldops below
void PlusOne(dota::bitstream &r, fieldpath* fp);
void PlusTwo(dota::bitstream &r, fieldpath* fp);
void PlusThree(dota::bitstream &r, fieldpath* fp);
void PlusFour(dota::bitstream &r, fieldpath* fp);
void PlusN(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaZeroRightZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaZeroRightNonZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaOneRightZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaOneRightNonZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaNRightZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaNRightNonZero(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaNRightNonZeroPack6Bits(dota::bitstream &r, fieldpath* fp);
void PushOneLeftDeltaNRightNonZeroPack8Bits(dota::bitstream &r, fieldpath* fp);
void PushTwoLeftDeltaZero(dota::bitstream &r, fieldpath* fp);
void PushTwoLeftDeltaOne(dota::bitstream &r, fieldpath* fp);
void PushTwoLeftDeltaN(dota::bitstream &r, fieldpath* fp);
void PushTwoPack5LeftDeltaZero(dota::bitstream &r, fieldpath* fp);
void PushTwoPack5LeftDeltaOne(dota::bitstream &r, fieldpath* fp);
void PushTwoPack5LeftDeltaN(dota::bitstream &r, fieldpath* fp);
void PushThreeLeftDeltaZero(dota::bitstream &r, fieldpath* fp);
void PushThreeLeftDeltaOne(dota::bitstream &r, fieldpath* fp);
void PushThreeLeftDeltaN(dota::bitstream &r, fieldpath* fp);
void PushThreePack5LeftDeltaZero(dota::bitstream &r, fieldpath* fp);
void PushThreePack5LeftDeltaOne(dota::bitstream &r, fieldpath* fp);
void PushThreePack5LeftDeltaN(dota::bitstream &r, fieldpath* fp);
void PushN(dota::bitstream &r, fieldpath* fp);
void PushNAndNonTopological(dota::bitstream &r, fieldpath* fp);
void PopOnePlusOne(dota::bitstream &r, fieldpath* fp);
void PopOnePlusN(dota::bitstream &r, fieldpath* fp);
void PopAllButOnePlusOne(dota::bitstream &r, fieldpath* fp);
void PopAllButOnePlusN(dota::bitstream &r, fieldpath* fp);
void PopAllButOnePlusNPackN(dota::bitstream &r, fieldpath* fp);
void PopAllButOnePlusNPack3Bits(dota::bitstream &r, fieldpath* fp);
void PopAllButOnePlusNPack6Bits(dota::bitstream &r, fieldpath* fp);
void PopNPlusOne(dota::bitstream &r, fieldpath* fp);
void PopNPlusN(dota::bitstream &r, fieldpath* fp);
void PopNAndNonTopographical(dota::bitstream &r, fieldpath* fp);
void NonTopoComplex(dota::bitstream &r, fieldpath* fp);
void NonTopoPenultimatePlusOne(dota::bitstream &r, fieldpath* fp);
void NonTopoComplexPack4Bits(dota::bitstream &r, fieldpath* fp);
void FieldPathEncodeFinish(dota::bitstream &r, fieldpath* fp);

#endif /* _FIELDPATH_HPP_ */