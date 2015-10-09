#include "fieldpath.hpp"

fieldpath newFieldpath(dt* parentTbl, HuffmanTree* huf) {
  fieldpath fp = {
    parentTbl,
    std::vector<fieldpath_field>(),
    std::vector<int>(),
    huf,
    false
  };
  
  fp.index.push_back(-1);
  return fp;
}

void walk(dota::bitstream &r, fieldpath* fp) {
  int cnt = 0;
  HuffmanTree* root = fp->tree;
  //print_node(root);
  HuffmanTree* node = root;
  //print_node(node);
  while (!fp->finished) {
    ++cnt;
    if (r.read(1) == 1) {
      //std::cout << "if true\n";
      HuffmanTree* i = (*node).right;
      if (i->isLeaf) {
        //std::cout << "is leaf\n";
        node = root;
        //std::cout << "i.Value(): " << std::to_string(i->value) << "\n";
        //std::cout << "fieldpathOp: " << fieldpathLookup[i->value].name << "\n";
        fieldpathLookup[i->value].fn(r, fp);
        if (!(fp->finished)) {
          addField(fp);
          //std::cout << "Reached in " << std::to_string(cnt) << " bits, " << fp->fields[fp->fields.size() - 1].name << ", " << std::to_string(r.position()) << "\n";
        }
        
        cnt = 0;
      }
      else {
        //std::cout << "not leaf\n";
        node = i;
      }
    }
    else {
      //std::cout << "if false\n";
      HuffmanTree* i = (*node).left;
      if (i->isLeaf) {
        //std::cout << "is leaf\n";
        node = root;
        //std::cout << "i.Value(): " << std::to_string(i->value) << "\n";
        //std::cout << "fieldpathOp: " << fieldpathLookup[i->value].name << "\n";
        fieldpathLookup[i->value].fn(r, fp);
        if (!(fp->finished)) {
          addField(fp);
          //std::cout << "Reached in " << std::to_string(cnt) << " bits, " << fp->fields[fp->fields.size() - 1].name << ", " << std::to_string(r.position()) << "\n";
        }
        
        cnt = 0;
      }
      else {
        //std::cout << "not leaf\n";
        node = i;
      }
    }

  }
}

void addField(fieldpath* fp) {
  dt* cDt = fp->parent;
  std::string name;
  int i;
  
  /*if debugLevel >= 6 {
    std::string path;
    for (int j = 0; j < fp->index.size() - 1; ++j) {
      path += std::to_string(fp->index[i]) + "/";
    }
    
    //std::cout << "Adding field with path: " << path << fp->index[fp->fields.size() - 1] << "\n";
  }*/
  //std::cout << "fp->index.size(): " << std::to_string(fp->index.size()) << "\n";
  for (i = 0; i < (int)fp->index.size() - 1; ++i) {
    //std::cout << "i: " << std::to_string(i) << "\n";
    //std::cout << "cDt name: " << cDt->name << "\n";
    //std::cout << "fp.index[i]: " << std::to_string(fp->index[i]) << "\n";
    //std::cout << "cDt.Properties len: " << std::to_string(cDt->properties.size()) << "\n";
    if (cDt->properties[fp->index[i]].table != nullptr) {
      //std::cout << "table name: " << cDt->properties[fp->index[i]].table->name << "\n";
      cDt = cDt->properties[fp->index[i]].table;
      name += cDt->name + ".";
    }
    else {
      ////std::cout << "i: " << std::to_string(i) << "\n";
      ////std::cout << "cDt->properties size: " << std::to_string(cDt->properties.size()) << "\n";
      ////std::cout << "fp->index[i]: " << std::to_string(fp->index[i]) << "\n";
      //std::cout << "expected table in fp properties: " << std::to_string(i) << ", " << cDt->properties[fp->index[i]].field.name << ", " << cDt->properties[fp->index[i]].field.type << "\n";
    }
  }
  //std::cout << "cDt->properties size: " << std::to_string(cDt->properties.size()) << "\n";
  //std::cout << "i: " << std::to_string(i) << "\n";
  //std::cout << "name: " << name << "\n";
  //std::cout << "fp->index[i]: " << std::to_string(fp->index[i]) << "\n";
  //std::cout << "cDt.Properties[fp.index[i]].Field.Name: " << cDt->properties[fp->index[i]].field.name << "\n";
  fieldpath_field new_fieldpath_field = {
    name + cDt->properties[fp->index[i]].field.name,
    &(cDt->properties[fp->index[i]].field)
  };
  fp->fields.push_back(new_fieldpath_field);
}

HuffmanTree newFieldpathHuffman() {
  int huffmanlist[fieldpathLookup.size()];
  for (int i = 0; i < (int)fieldpathLookup.size(); ++i) {
    huffmanlist[i] = fieldpathLookup[i].weight;
  }
  return buildTree(huffmanlist, fieldpathLookup.size());
}

void PlusOne(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 1;
}
void PlusTwo(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 2;
}
void PlusThree(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 3;
}
void PlusFour(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 4;
}
void PlusN(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVarFP() + 5;
}
void PushOneLeftDeltaZeroRightZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back(0);
}
void PushOneLeftDeltaZeroRightNonZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushOneLeftDeltaOneRightZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 1;
  fp->index.push_back(0);
}
void PushOneLeftDeltaOneRightNonZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += 1;
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushOneLeftDeltaNRightZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVarFP();
  fp->index.push_back(0);
}
void PushOneLeftDeltaNRightNonZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVarFP() + 2;
  fp->index.push_back((int)r.nReadUBitVarFP() + 1);
}
void PushOneLeftDeltaNRightNonZeroPack6Bits(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.read(3) + 2;
  fp->index.push_back((int)r.read(3) + 1);
}
void PushOneLeftDeltaNRightNonZeroPack8Bits(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.read(4) + 2;
  fp->index.push_back((int)r.read(4) + 1);
}
void PushTwoLeftDeltaZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushTwoLeftDeltaOne(dota::bitstream &r, fieldpath* fp) {
  ++(fp->index.back());
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushTwoLeftDeltaN(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVar() + 2;
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushTwoPack5LeftDeltaZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushTwoPack5LeftDeltaOne(dota::bitstream &r, fieldpath* fp) {
  ++(fp->index.back());
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushTwoPack5LeftDeltaN(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVar() + 2;
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushThreeLeftDeltaZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushThreeLeftDeltaOne(dota::bitstream &r, fieldpath* fp) {
  ++(fp->index.back());
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushThreeLeftDeltaN(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVar() + 2;
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
  fp->index.push_back((int)r.nReadUBitVarFP());
}
void PushThreePack5LeftDeltaZero(dota::bitstream &r, fieldpath* fp) {
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushThreePack5LeftDeltaOne(dota::bitstream &r, fieldpath* fp) {
  ++(fp->index.back());
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushThreePack5LeftDeltaN(dota::bitstream &r, fieldpath* fp) {
  fp->index.back() += (int)r.nReadUBitVar() + 2;
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
  fp->index.push_back((int)r.read(5));
}
void PushN(dota::bitstream &r, fieldpath* fp) {
  int n = (int)r.nReadUBitVar();
  fp->index.back() += (int)r.nReadUBitVar();
  for (int i = 0; i < n; ++i) {
    fp->index.push_back((int)r.nReadUBitVarFP());
  }
}
void PushNAndNonTopological(dota::bitstream &r, fieldpath* fp) {
  for (int i = 0; i < (int)fp->index.size(); ++i) {
    if (r.read(1) != 0) {
      fp->index[i] += (int)r.nReadVarSInt32() + 1;
    }
  }
  
  int count = (int)r.nReadUBitVar();
  for (int j = 0; j < count; ++j) {
    fp->index.push_back((int)r.nReadUBitVarFP());
  }
}
void PopOnePlusOne(dota::bitstream &r, fieldpath* fp) {
  fp->index.pop_back();
  ++(fp->index.back());
}
void PopOnePlusN(dota::bitstream &r, fieldpath* fp) {
  fp->index.pop_back();
  fp->index.back() += (int)r.nReadUBitVarFP() + 1;
}
void PopAllButOnePlusOne(dota::bitstream &r, fieldpath* fp) {
  std::vector<int> new_index {fp->index[0]};
  fp->index = new_index;
  ++(fp->index.back());
}
void PopAllButOnePlusN(dota::bitstream &r, fieldpath* fp) {
  std::vector<int> new_index {fp->index[0]};
  fp->index = new_index;
  fp->index.back() += (int)r.nReadUBitVarFP() + 1;
}

void PopAllButOnePlusNPackN(dota::bitstream &r, fieldpath* fp) {}

void PopAllButOnePlusNPack3Bits(dota::bitstream &r, fieldpath* fp) {
  std::vector<int> new_index {fp->index[0]};
  fp->index = new_index;
  fp->index.back() += (int)r.read(3) + 1;
}
void PopAllButOnePlusNPack6Bits(dota::bitstream &r, fieldpath* fp) {
  std::vector<int> new_index {fp->index[0]};
  fp->index = new_index;
  fp->index.back() += (int)r.read(6) + 1;
}
void PopNPlusOne(dota::bitstream &r, fieldpath* fp) {
  //std::cout << "PopNPlusOne\n";
  int i = (int)r.nReadUBitVarFP();
  //std::cout << "i: " << std::to_string(i) << "\n";
  //std::cout << "fp index size: " << std::to_string(fp->index.size()) << "\n";
  std::vector<int> new_index(fp->index.begin(), fp->index.end() - i);
  //std::cout << "fp index size: " << std::to_string(fp->index.size()) << "\n";
  fp->index = new_index;
  //std::cout << "PopNPlusOne 1\n";
  ++(fp->index.back());
  //std::cout << "PopNPlusOne end\n";
}
void PopNPlusN(dota::bitstream &r, fieldpath* fp) {
  std::vector<int> new_index(fp->index.begin(), fp->index.end() - (int)r.nReadUBitVarFP());
  fp->index = new_index;
  fp->index.back() += (int)r.nReadVarSInt32();
}
void PopNAndNonTopographical(dota::bitstream &r, fieldpath* fp) {
  //std::cout << "PopNAndNonTopographical\n";
  int i = (int)r.nReadUBitVarFP();
  //std::cout << "i: " << std::to_string(i) << "\n";
  //std::cout << "fp index size: " << std::to_string(fp->index.size()) << "\n";
  std::vector<int> new_index(fp->index.begin(), fp->index.end() - i);
  //std::cout << "fp index size: " << std::to_string(fp->index.size()) << "\n";
  fp->index = new_index;
  //std::cout << "PopNAndNonTopographical 1\n";
  for (int i = 0; i < (int)fp->index.size(); ++i) {
    if (r.read(1) != 0) {
      fp->index[i] += (int)r.nReadVarSInt32();
    }
  }
  //std::cout << "end fp index size: " << std::to_string(fp->index.size()) << "\n";
}
void NonTopoComplex(dota::bitstream &r, fieldpath* fp) {
  for (int i = 0; i < (int)fp->index.size(); ++i) {
    if (r.read(1) != 0) {
      fp->index[i] += (int)r.nReadVarSInt32();
    }
  }
}
void NonTopoPenultimatePlusOne(dota::bitstream &r, fieldpath* fp) {
  ++(fp->index[fp->index.size() - 2]);
}
void NonTopoComplexPack4Bits(dota::bitstream &r, fieldpath* fp) {
  for (int i = 0; i < (int)fp->index.size(); ++i) {
    if (r.read(1) != 0) {
      fp->index[i] += (int)r.read(4) - 7;
    }
  }
}
void FieldPathEncodeFinish(dota::bitstream &r, fieldpath* fp) {
  fp->finished = true;
}

std::vector<fieldpathOp> fieldpathLookup = {
	{"PlusOne", &PlusOne, 36271},
	{"PlusTwo", &PlusTwo, 10334},
	{"PlusThree", &PlusThree, 1375},
	{"PlusFour", &PlusFour, 646},
	{"PlusN", &PlusN, 4128},
	{"PushOneLeftDeltaZeroRightZero", &PushOneLeftDeltaZeroRightZero, 35},
	{"PushOneLeftDeltaZeroRightNonZero", &PushOneLeftDeltaZeroRightNonZero, 3},
	{"PushOneLeftDeltaOneRightZero", &PushOneLeftDeltaOneRightZero, 521},
	{"PushOneLeftDeltaOneRightNonZero", &PushOneLeftDeltaOneRightNonZero, 2942},
	{"PushOneLeftDeltaNRightZero", &PushOneLeftDeltaNRightZero, 560},
	{"PushOneLeftDeltaNRightNonZero", &PushOneLeftDeltaNRightNonZero, 471},
	{"PushOneLeftDeltaNRightNonZeroPack6Bits", &PushOneLeftDeltaNRightNonZeroPack6Bits, 10530},
	{"PushOneLeftDeltaNRightNonZeroPack8Bits", &PushOneLeftDeltaNRightNonZeroPack8Bits, 251},
	{"PushTwoLeftDeltaZero", &PushTwoLeftDeltaZero, 0},
	{"PushTwoPack5LeftDeltaZero", &PushTwoPack5LeftDeltaZero, 0},
	{"PushThreeLeftDeltaZero", &PushThreeLeftDeltaZero, 0},
	{"PushThreePack5LeftDeltaZero", &PushThreePack5LeftDeltaZero, 0},
	{"PushTwoLeftDeltaOne", &PushTwoLeftDeltaOne, 0},
	{"PushTwoPack5LeftDeltaOne", &PushTwoPack5LeftDeltaOne, 0},
	{"PushThreeLeftDeltaOne", &PushThreeLeftDeltaOne, 0},
	{"PushThreePack5LeftDeltaOne", &PushThreePack5LeftDeltaOne, 0},
	{"PushTwoLeftDeltaN", &PushTwoLeftDeltaN, 0},
	{"PushTwoPack5LeftDeltaN", &PushTwoPack5LeftDeltaN, 0},
	{"PushThreeLeftDeltaN", &PushThreeLeftDeltaN, 0},
	{"PushThreePack5LeftDeltaN", &PushThreePack5LeftDeltaN, 0},
	{"PushN", &PushN, 0},
	{"PushNAndNonTopological", &PushNAndNonTopological, 310},
	{"PopOnePlusOne", &PopOnePlusOne, 2},
	{"PopOnePlusN", &PopOnePlusN, 0},
	{"PopAllButOnePlusOne", &PopAllButOnePlusOne, 1837},
	{"PopAllButOnePlusN", &PopAllButOnePlusN, 149},
	{"PopAllButOnePlusNPack3Bits", &PopAllButOnePlusNPack3Bits, 300},
	{"PopAllButOnePlusNPack6Bits", &PopAllButOnePlusNPack6Bits, 634},
	{"PopNPlusOne", &PopNPlusOne, 0},
	{"PopNPlusN", &PopNPlusN, 0},
	{"PopNAndNonTopographical", &PopNAndNonTopographical, 1},
	{"NonTopoComplex", &NonTopoComplex, 76},
	{"NonTopoPenultimatePlusOne", &NonTopoPenultimatePlusOne, 271},
	{"NonTopoComplexPack4Bits", &NonTopoComplexPack4Bits, 99},
	{"FieldPathEncodeFinish", &FieldPathEncodeFinish, 25474},
};

HuffmanTree huf = newFieldpathHuffman();