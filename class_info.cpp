#include "class_info.hpp"

std::regex gameBuildRegexp = std::regex("/dota_v(\\d+)/");

void Parser::onCSVCMsg_ServerInfo(std::string raw_data) {
  CSVCMsg_ServerInfo data;
  data.ParseFromString(raw_data);
  
  classIdSize = (int)log2((int)data.max_classes());
  std::smatch match;
  std::cout << "The following matches gameBuildRegexp: " << data.game_dir() << std::endl;
  std::regex_search (data.game_dir(), match, gameBuildRegexp);
  uint32_t build = (uint32_t)std::stoul(match[1], nullptr);
  std::cout << "build: " << std::to_string(build) << std::endl;
  GameBuild = build;
}

uint32_t Parser::updateInstanceBaseline() {
  std::cout << "updateInstanceBaseline\n";
  StringTable stringTable = stringTables.tables[stringTables.nameIndex["instancebaseline"]];
  if (!hasClassInfo) return 0;
  for(auto const &ent : stringTable.items) {
    updateInstanceBaselineItem(ent.second);
  }
  return 0;
}

uint32_t Parser::updateInstanceBaselineItem(StringTableItem item) {
  std::cout << "updateInstanceBaselineItem\n";
  int classId = std::stoi(item.key);
  std::cout << "classId: " << std::to_string(classId) << "\n";
  std::string className = classInfo[classId];
  std::cout << "className: " << className << "\n";
  if (classBaselines.find(classId) == classBaselines.end()) {
    //p->classBaselines[classId] = //new properties
    std::cout << "new properties: " << std::to_string(classId) << "\n";
  }
  std::map<int, dt> serializer = serializers[className];
  std::cout << "serializer name: " << serializer[0].name << "\n";
  if (item.value.length() > 0) {
    std::cout << "item.value.length(): " << std::to_string(item.value.length()) << "\n";
    dota::bitstream stream(item.value);
    std::cout << "stream size: " << std::to_string(stream.end()) << "\n";
    classBaselines[classId] = readProperties(stream, serializer[0]);
  }
  return 0;
}