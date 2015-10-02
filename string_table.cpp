#include "string_table.hpp"

void Parser::onCSVCMsg_CreateStringTable(std::string raw_data) {
  CSVCMsg_CreateStringTable data;
  data.ParseFromString(raw_data);
      
  std::cout << "nextIndex: " << std::to_string(stringTables.nextIndex) << "\n";
  std::cout << "name: " << data.name() << "\n";
  std::cout << "user_data_fixed_size: " << std::to_string(data.user_data_fixed_size()) << "\n";
  std::cout << "user_data_size: " << std::to_string(data.user_data_size()) << "\n";
  std::map<int, StringTableItem> stringTableItems;
  StringTable string_table = {
    stringTables.nextIndex,
    data.name(),
    stringTableItems,
    data.user_data_fixed_size(),
    data.user_data_size()
  };
  stringTables.nextIndex += 1;
  std::string buffer = data.string_data();
  std::cout << "is_compressed: " << std::to_string(data.data_compressed()) << "\n";
  std::vector<StringTableItem> items;
  if (data.data_compressed()) {
    std::size_t uSize;
    if (snappy::GetUncompressedLength(buffer.c_str(), buffer.length(), &uSize)) {
      char uBuffer[uSize];
      if (snappy::RawUncompress(buffer.c_str(), buffer.length(), uBuffer)) {
        std::cout << "uncompressed success, size: " << std::to_string(uSize) << "\n";
        items = parseStringTable(uBuffer, uSize, data.num_entries(), string_table.userDataFixedSize, string_table.userDataSize);
      }
    }
  }
  else {
    items = parseStringTable(buffer.c_str(), buffer.length(), data.num_entries(), string_table.userDataFixedSize, string_table.userDataSize);
  }
  for (std::vector<StringTableItem>::iterator it = items.begin(); it != items.end(); ++it) {
    string_table.items[it->index] = *it;
  }
  stringTables.tables[string_table.index] = string_table;
  stringTables.nameIndex[string_table.name] = string_table.index;
  if (string_table.name.compare("instancebaseline") == 0) {
    updateInstanceBaseline();
  }
}

std::vector<StringTableItem> parseStringTable(const char* buffer, int buffer_size, int num_updates, bool userDataFixed, int userDataSize) {
  std::vector<StringTableItem> items;
  if (buffer_size == 0) return items;
  dota::bitstream stream(std::string(buffer, buffer_size));
  uint32_t index = -1;
  std::vector<std::string> keys;
  
	// Loop through entries in the data structure
	//
	// Each entry is a tuple consisting of {index, key, value}
	//
	// Index can either be incremented from the previous position or
	// overwritten with a given entry.
	//
	// Key may be omitted (will be represented here as "")
	//
	// Value may be omitted
  for (int i = 0; i < num_updates; ++i) {
    std::string key = "";
    std::string value;
    
		// Read a boolean to determine whether the operation is an increment or
		// has a fixed index position. A fixed index position of zero should be
		// the last data in the buffer, and indicates that all data has been read.
    uint32_t incr = stream.read(1);
    if (incr) {
      ++index;
    }
    else {
      index = stream.nReadVarUInt32() + 1;
    }
    std::cout << "index: " << std::to_string(index) << "\n";
    
    // Some values have keys, some don't.
    bool hasKey = stream.read(1);
    std::cout << "hasKey: " << std::to_string(hasKey) << "\n";
    if (hasKey) {
			// Some entries use reference a position in the key history for
			// part of the key. If referencing the history, read the position
			// and size from the buffer, then use those to build the string
			// combined with an extra string read (null terminated).
			// Alternatively, just read the string.
      bool useHistory = stream.read(1);
      std::cout << "useHistory: " << std::to_string(useHistory) << "\n";
      char tmpBuf[1024];
      if (useHistory) {
        uint32_t pos = stream.read(5);
        uint32_t size = stream.read(5);
        stream.nReadString(tmpBuf, 1024);
        if (pos >= keys.size()) {
          key += std::string(tmpBuf);
        }
        else {
          std::string s = keys[pos];
          if (size > s.length()) {
            key += s + std::string(tmpBuf);
          }
          else {
            key += s.substr(0, size) + std::string(tmpBuf);
          }
        }
      }
      else {
        stream.nReadString(tmpBuf, 1024);
        key = std::string(tmpBuf);
      }
      
      if (keys.size() >= 32) {
        keys.erase(keys.begin());
      }
      std::cout << "key: " << key << "\n";
      keys.push_back(key);
    }
    
    // Some entries have a value.
    bool hasValue = stream.read(1);
    std::cout << "hasValue: " << std::to_string(hasValue) << "\n";
    if (hasValue) {
			// Values can be either fixed size (with a size specified in
			// bits during table creation, or have a variable size with
			// a 14-bit prefixed size.
      if (userDataFixed) {
        char tmpBuf[(int)ceil(userDataSize / 8)];
        stream.readBits(tmpBuf, userDataSize);
        value = std::string(tmpBuf, (int)ceil(userDataSize / 8));
      }
      else {
        uint32_t size = stream.read(14);
        std::cout << "value size: " << std::to_string(size) << "\n";
        stream.read(3);
        char tmpBuf[size];
        stream.readBits(tmpBuf, size * 8);
        value = std::string(tmpBuf, size);
      }
      std::cout << "value: " << value << "\n";
    }
    StringTableItem item = {
      (int)index,
      key,
      value
    };
    items.push_back(item);
  }
  return items;
}