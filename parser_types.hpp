#ifndef _PARSER_TYPES_HPP_
#define _PARSER_TYPES_HPP_

#include <string>
#include <map>
#include "boost/variant.hpp"
#include "bitstream.hpp"

// Holds and maintains the string table information for an
// instance of the Parser.
struct StringTables {
  std::map<int, struct StringTable> tables;
  std::map<std::string, int> nameIndex;
	int nextIndex;
  StringTables() {
    nextIndex = 0;
  }
};

// Holds and maintains the information for a string table.
struct StringTable {
	int index;
	std::string name;
	std::map<int, struct StringTableItem> items;
	bool userDataFixedSize;
	int userDataSize;
};

struct StringTableItem {
  int index;
  std::string key;
  std::string value;
};

typedef boost::variant<
  bool,
  char,
  float,
  double,
  uint16_t,
  uint32_t,
  uint64_t,
  int32_t,
  int64_t,
  std::string,
  std::vector<float>
> value_type;

struct dt_field;

struct Properties {
  std::map<std::string, value_type> KV;
};

struct PropertySerializer {
  value_type (*decode)(dota::bitstream &stream, dt_field* f);
  value_type (*decodeContainer)(dota::bitstream &stream, dt_field* f);
  bool isArray;
  uint32_t length;
  PropertySerializer* arraySerializer;
  std::string name;
  /*PropertySerializer() {
    decode = nullptr;
    decodeContainer = nullptr;
  }*/
};

// A datatable field
struct dt_field {
  std::string name;
  std::string encoder;
  std::string type;
  int index;
  int flags;
  bool has_flags;
  int bit_count;
  bool has_bit_count;
  float low_value;
  bool has_low_value;
  float high_value;
  bool has_high_value;
  int version;
  PropertySerializer serializer;
  uint32_t build;
};

// Field is always filled, table only for sub-tables
struct dt_property {
  struct dt_field field;
  struct dt* table;
};

// A single datatable
struct dt {
  std::string name;
  int flags;
  int version;
  std::vector<dt_property> properties;
};

struct HuffmanTree {
	int weight;
	bool isLeaf;
	int value;
	HuffmanTree* left;
	HuffmanTree* right;
};

// A single field to be read
struct fieldpath_field {
	std::string name;
	dt_field* field;
};

// A fieldpath, used to walk through the flattened table hierarchy
struct fieldpath {
  dt* parent;
	std::vector<fieldpath_field> fields;
	std::vector<int> index;
	HuffmanTree* tree;
	bool finished;
};

// Contains the weight and lookup function for a single operation
struct fieldpathOp {
	std::string name;
  void (*fn)(dota::bitstream &stream, fieldpath* fp);
	int weight;
};

struct parser {
  std::map<int, std::string> classInfo;
  std::map<int, struct Properties> classBaselines;
  bool hasClassInfo;
  std::map< std::string, std::map<int, dt> > serializers;
  StringTables stringTables;
  uint32_t GameBuild;
  int classIdSize;
  parser() {
    hasClassInfo = false;
  }
};

struct pendingMessage {
  uint32_t tick;
  int type;
  std::string data;
};

#endif /* _PARSER_TYPES_HPP_ */