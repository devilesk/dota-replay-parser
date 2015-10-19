#include "class_info.hpp"

std::regex gameBuildRegexp = std::regex("/dota_v(\\d+)/");

void Parser::onCSVCMsg_ServerInfo(const char* buffer, int size) {
  CSVCMsg_ServerInfo data;
  data.ParseFromArray(buffer, size);
  
  // from log2 util.go
  classIdSize = (int)(log((double)data.max_classes())/log(2)) + 1;
  //std::cout << "set classIdSize: " << std::to_string(data.max_classes()) << std::endl;
  //std::cout << "set classIdSize: " << std::to_string((int)data.max_classes()) << std::endl;
  //std::cout << "set classIdSize: " << std::to_string(log2((int)data.max_classes())) << std::endl;
  //std::cout << "set classIdSize: " << std::to_string(classIdSize) << std::endl;
  std::smatch match;
  //std::cout << "The following matches gameBuildRegexp: " << data.game_dir() << std::endl;
  std::regex_search (data.game_dir(), match, gameBuildRegexp);
  uint32_t build = (uint32_t)std::stoul(match[1], nullptr);
  std::cout << "build: " << std::to_string(build) << std::endl;
  GameBuild = build;
}

void Parser::onCDemoClassInfo(const char* buffer, int size) {
  CDemoClassInfo data;
  data.ParseFromArray(buffer, size);
  
  // Iterate through items, storing the mapping in the parser state
  for (int i = 0; i < data.classes_size(); ++i) {
    classInfo[data.classes(i).class_id()] = data.classes(i).network_name();
    
    if (serializers.find(data.classes(i).network_name()) == serializers.end()) {
      std::cout << "unable to find table for class " << std::to_string(data.classes(i).class_id()) << " " << data.classes(i).network_name() << "\n";
    }
  }
  
  // Remember that we've gotten the class info
  hasClassInfo = true;
  
  // Try to update the instancebaseline
  updateInstanceBaseline();
}

uint32_t Parser::updateInstanceBaseline() {
  //std::cout << "updateInstanceBaseline\n";
  if (!hasClassInfo) return 0;
  if (stringTables.nameIndex.find("instancebaseline") == stringTables.nameIndex.end() || stringTables.tables.find(stringTables.nameIndex["instancebaseline"]) == stringTables.tables.end()) return 0;
  StringTable* stringTable = stringTables.tables[stringTables.nameIndex["instancebaseline"]];
  for(auto const &ent : stringTable->items) {
    updateInstanceBaselineItem(ent.second);
  }
  return 0;
}

uint32_t Parser::updateInstanceBaselineItem(StringTableItem* item) {
  //std::cout << "updateInstanceBaselineItem\n";
  int classId = std::stoi(item->key);
  //std::cout << "classId: " << std::to_string(classId) << "\n";
  std::string className = classInfo[classId];
  //std::cout << "className: " << className << "\n";
  /*if (classBaselines.find(classId) == classBaselines.end()) {
    //p->classBaselines[classId] = //new properties
    classBaselines[classId] = Properties();
    //std::cout << "new properties: " << std::to_string(classId) << "\n";
  }*/
  //std::cout << "serializer name: " << serializer[0].name << "\n";
  if (item->value.length() > 0) {
    //std::cout << "item.value.length(): " << std::to_string(item->value.length()) << "\n";
    dota::bitstream stream(item->value);
    //std::cout << "stream size: " << std::to_string(stream.end()) << "\n";
    if (classBaselines.find(classId) == classBaselines.end()) {
      delete classBaselines[classId];
    }
    classBaselines[classId] = readProperties(stream, serializers[className][0]);
  }
  else if (classBaselines.find(classId) == classBaselines.end()) {
    classBaselines[classId] = new Properties();
  }
  return 0;
}