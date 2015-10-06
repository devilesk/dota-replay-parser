#include "parser.hpp"

int main ()
{
  //std::cout << "HELLO WORLD!\n";
  Parser p;
  StringTables string_tables;
  p.stringTables = string_tables;
  char* buffer;
  std::ifstream stream;
  std::string path = "testfiles/1781962623_source2.dem";
  //std::string path = "testfiles/1698148651_source2.dem";
  // std::ifstream::in and std::ifstream::binary are mode flags for input and binary
  stream.open(path.c_str(), std::ifstream::in | std::ifstream::binary);
  if (!stream.is_open()) {
    //std::cout << "Could not open file.\n";
  }
  else {
    //std::cout << "File opened.\n";
  }

  // get file length
  stream.seekg(0, stream.end);
  int length = stream.tellg();
  stream.seekg(0, stream.beg);

  buffer = new char[1];
  char* header = new char[8];

  stream.read(header, sizeof(header));
  std::string demheader = "PBDEMS2";
  //std::cout << std::to_string(sizeof(header)) << "\n";
  //std::cout << header << "\n";

  if (strcmp(header, demheader.c_str())) {
    //std::cout << "Invalid header.\n";
  }
  else {
    //std::cout << "Valid header.\n";
  }

  delete buffer;
  delete header;

  // Skip the next 8 bytes, which appear to be two int32s related to the size
  // of the demo file. We may need them in the future, but not so far.
  stream.seekg(8, stream.cur);

  uint32_t cmd;
  cmd = readVarUInt32(stream);
  //std::cout << "command: " << std::to_string(cmd) << "\n";

  uint32_t tick;
  tick = readVarUInt32(stream);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  //std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(stream);
  //std::cout << "size: " << std::to_string(size) << "\n";

  buffer = new char[size];
  stream.read(buffer, size);

  CDemoFileHeader demo_header;
  demo_header.ParseFromArray(buffer, size);
  //std::cout << "map_name: " << demo_header.map_name() << "\n";

  delete buffer;

  // read dem messages
  while (stream.good())
  {
    p.readMessage(stream);
  }

  //std::cout << "File length: " << std::to_string(length) << "\n";
  stream.close();
  //std::cout << "File closed.\n";
}

uint32_t Parser::readMessage(std::ifstream &stream) {
  char* buffer;
  char* uBuffer;
  uint32_t cmd;
  cmd = readVarUInt32(stream);
  const bool compressed = cmd & 112;
  cmd = (cmd & ~112);
  //std::cout << "command: " << std::to_string(cmd) << " compressed: " << std::to_string(compressed) << "\n";

  uint32_t tick;
  tick = readVarUInt32(stream);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  //std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(stream);
  //std::cout << "size: " << std::to_string(size) << "\n";

  buffer = new char[size];
  stream.read(buffer, size);

  if (compressed && snappy::IsValidCompressedBuffer(buffer, size)) {
    //std::cout << "valid snappy compressed buffer\n";
    std::size_t uSize;
    if (snappy::GetUncompressedLength(buffer, size, &uSize)) {
      uBuffer = new char[uSize];
      if (snappy::RawUncompress(buffer, size, uBuffer)) {
        //std::cout << "uncompressed success, size: " << std::to_string(uSize) << "\n";
        parseMessage(cmd, tick, uSize, uBuffer);
      }
      else {
        //std::cout << "uncompress fail\n";
        exit(0);
      }
      delete uBuffer;
    }
    else {
      //std::cout << "get uncompressed length fail\n";
      exit(0);
    }
  }
  else {
    parseMessage(cmd, tick, size, buffer);
  }
  
  delete buffer;
}

uint32_t Parser::parseMessage(int cmd, int _tick, int size, char* buffer) {
  tick = _tick;
  if (cmd == 4) {
    //std::cout << "type: DEM_SendTables\n";
    //CDemoSendTables sendTables;
    //sendTables.ParseFromArray(buffer, size);
    //parseSendTables(&sendTables, getDefaultPropertySerializerTable());
    std::string data(buffer, size);
    onCDemoSendTables(data);
  }
  else if (cmd == 5) {
    //std::cout << "type: DEM_ClassInfo\n";
    CDemoClassInfo classInfo;
    classInfo.ParseFromArray(buffer, size);
    parseClassInfo(&classInfo);
  }
  else if (cmd == 6) {
    //std::cout << "type: DEM_StringTables\n";
    CDemoStringTables stringTables;
    stringTables.ParseFromArray(buffer, size);
    //parseStringTables(&stringTables, p);
  }
  else if (cmd == 7) {
    //std::cout << "type: DEM_Packet\n";
    CDemoPacket packet;
    packet.ParseFromArray(buffer, size);
    onCDemoPacket(&packet, _tick);
    /*do
    {
      //std::cout << "Press a key to continue...\n";
    } while (std::cin.get() != '\n');*/
  }
  else if (cmd == 8) {
    //std::cout << "type: DEM_SignonPacket\n";
    CDemoPacket packet;
    packet.ParseFromArray(buffer, size);
    onCDemoPacket(&packet, _tick);
    /*do
    {
      //std::cout << "Press a key to continue...\n";
    } while (std::cin.get() != '\n');*/
  }
  else if (cmd == 13) {
    //std::cout << "type: DEM_FullPacket\n";
    CDemoFullPacket packet;
    packet.ParseFromArray(buffer, size);
    onCDemoFullPacket(&packet, _tick);
  }
  return 0;
}

uint32_t Parser::parseClassInfo(CDemoClassInfo* _classInfo) {
  for (int i = 0; i < _classInfo->classes_size(); ++i) {
    CDemoClassInfo_class_t c = _classInfo->classes(i);
    classInfo[c.class_id()] = c.network_name();
  }
  hasClassInfo = true;
  updateInstanceBaseline();
  return 0;
}

uint32_t Parser::parseStringTables(const CDemoStringTables* stringTables) {
  for (int i = 0; i < stringTables->tables_size(); ++i) {
    CDemoStringTables_table_t t = stringTables->tables(i);
    //std::cout << "table_name: " << t.table_name() << "\n";
    for (int j = 0; j < t.items_size(); ++j) {
      CDemoStringTables_items_t item = t.items(j);
      //std::cout << "str: " << item.str() << "\n";
      uint32_t size;
      int pos = 0;
      size = readVarUInt32(item.data().c_str(), &pos);
      //std::cout << "data size: " << std::to_string(size) << "\n";
      if (size > 4) {
        //std::cout << "data: ";
        for (int k = 0; k < 4; ++k) {
          //std::cout << item.data().c_str()[pos + k];
        }
        //std::cout << "\n";
        do
        {
          //std::cout << "Press a key to continue...\n";
        } while (std::cin.get() != '\n');
      }
    }
    for (int j = 0; j < t.items_clientside_size(); ++j) {
      CDemoStringTables_items_t item = t.items_clientside(j);
      //std::cout << "str: " << item.str() << "\n";
    }
    //std::cout << "items_size: " << std::to_string(t.items_size()) << "\n";
    //std::cout << "items_clientside_size: " << std::to_string(t.items_clientside_size()) << "\n";
    //std::cout << "table_flags: " << std::to_string(t.table_flags()) << "\n";
    do
    {
      //std::cout << "Press a key to continue...\n";
    } while (std::cin.get() != '\n');
  }
  return 0;
}

int packet_priority(int type) {
  switch(type) {
    case NET_Messages::net_Tick:
    case SVC_Messages::svc_CreateStringTable:
    case SVC_Messages::svc_UpdateStringTable:
    case NET_Messages::net_SpawnGroup_Load:
      return -10;
    break;
    case SVC_Messages::svc_PacketEntities:
      return 5;
    break;
    case EBaseGameEvents::GE_Source1LegacyGameEvent:
      return 10;
    break;
  }
  return 0;
}

bool compare_packet_priority(pendingMessage i,pendingMessage j) {
  if (i.tick > j.tick) return false;
  if (i.tick < j.tick) return true;
  return packet_priority(i.type) < packet_priority(j.type);
}

void Parser::onCDemoPacket(const CDemoPacket* packet, int tick) {
  dota::bitstream stream(packet->data());
  uint32_t ret;
  int type;
  uint32_t size;
  std::vector<pendingMessage> pendingMessages;
  while (stream.end() - stream.position() >= 8) {
    type = stream.nReadUBitVar();
    size = stream.nReadVarUInt32();
    ////std::cout << "type: " << std::to_string(type) << "\n";
    ////std::cout << "size: " << std::to_string(size) << "\n";
    char buffer[8 * size];
    stream.readBits(buffer, 8 * size);
    pendingMessage msg;
    msg.type = type;
    msg.tick = tick;
    msg.data = std::string(&buffer[0], size);
    pendingMessages.push_back(msg);
  }
  std::stable_sort(pendingMessages.begin(), pendingMessages.end(), compare_packet_priority);
  for (std::vector<pendingMessage>::iterator it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
    ////std::cout << "priority: " << std::to_string(packet_priority(it->type)) << "\n";
    //std::cout << "TICK: " << std::to_string(tick) << "\n";
    parsePendingMessage(&*it);
  }
}

void Parser::onCDemoFullPacket(const CDemoFullPacket* packet, int tick) {
  //parseStringTables(&(packet->string_table()), p);
  onCDemoPacket(&(packet->packet()), tick);
}
