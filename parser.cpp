#include "parser.hpp"

int main ()
{
  //std::cout << "HELLO WORLD!\n";
  Parser p;
  //StringTables string_tables;
  //p.stringTables = string_tables;
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

  char * buffer = new char[length];
  
  stream.read(buffer, length);
  stream.close();
  
  char header[8];

  std::copy(buffer, &buffer[7], header);
  std::string demheader = "PBDEMS2";
  //std::cout << std::to_string(sizeof(header)) << "\n";
  //std::cout << header << "\n";

  if (strcmp(header, demheader.c_str())) {
    //std::cout << "Invalid header.\n";
  }
  else {
    //std::cout << "Valid header.\n";
  }

  int pos = 8;

  // Skip the next 8 bytes, which appear to be two int32s related to the size
  // of the demo file. We may need them in the future, but not so far.
  pos += 8;

  uint32_t cmd;
  cmd = readVarUInt32(buffer, pos);
  //std::cout << "command: " << std::to_string(cmd) << "\n";

  uint32_t tick;
  tick = readVarUInt32(buffer, pos);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  //std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(buffer, pos);
  //std::cout << "size: " << std::to_string(size) << "\n";

  CDemoFileHeader demo_header;
  demo_header.ParseFromArray(&buffer[pos], size);
  //std::cout << "map_name: " << demo_header.map_name() << "\n";

  pos += size;
  
  // read dem messages
  while (pos < length)
  {
    p.readMessage(buffer, pos);
  }

  //std::cout << "File length: " << std::to_string(length) << "\n";
  delete[] buffer;
  //std::cout << "File closed.\n";
  
  std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
}

void Parser::readMessage(const char* buffer, int &pos) {
  char* uBuffer;
  uint32_t cmd;
  cmd = readVarUInt32(buffer, pos);
  const bool compressed = (cmd & EDemoCommands::DEM_IsCompressed) == EDemoCommands::DEM_IsCompressed;
  cmd = (cmd & ~EDemoCommands::DEM_IsCompressed);
  //std::cout << "command: " << std::to_string(cmd) << " compressed: " << std::to_string(compressed) << "\n";

  uint32_t tick;
  tick = readVarUInt32(buffer, pos);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  //std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(buffer, pos);
  //std::cout << "size: " << std::to_string(size) << "\n";

  //if (compressed && snappy::IsValidCompressedBuffer(&buffer[pos], size)) {
  if (compressed) {
    //std::cout << "valid snappy compressed buffer\n";
    std::size_t uSize;
    if (snappy::GetUncompressedLength(&buffer[pos], size, &uSize)) {
      char * uBuffer = new char[uSize];
      if (snappy::RawUncompress(&buffer[pos], size, uBuffer)) {
        //std::cout << "uncompressed success, size: " << std::to_string(uSize) << "\n";
        parseMessage(cmd, tick, uSize, uBuffer);
      }
      else {
        //std::cout << "uncompress fail\n";
        exit(0);
      }
      delete[] uBuffer;
    }
    else {
      //std::cout << "get uncompressed length fail\n";
      exit(0);
    }
  }
  else {
    parseMessage(cmd, tick, size, &buffer[pos]);
  }
  
  pos += size;
}

void Parser::parseMessage(int cmd, int _tick, int size, const char* buffer) {
  tick = _tick;
  CDemoPacket packet;
  switch(cmd) {
    case 4:
      //std::cout << "type: DEM_SendTables\n";
      onCDemoSendTables(buffer, size);
    break;
    case 5:
      //std::cout << "type: DEM_ClassInfo\n";
      onCDemoClassInfo(buffer, size);
    break;
    case 6:
      //std::cout << "type: DEM_StringTables\n";
      //CDemoStringTables stringTables;
      //stringTables.ParseFromArray(buffer, size);
    break;
    case 7:
      //std::cout << "type: DEM_Packet\n";
    case 8:
      //std::cout << "type: DEM_SignonPacket\n";
      packet.ParseFromArray(buffer, size);
      onCDemoPacket(&packet);
    break;
    case 13:
      //std::cout << "type: DEM_FullPacket\n";
      CDemoFullPacket packet;
      packet.ParseFromArray(buffer, size);
      onCDemoFullPacket(&packet);
    break;
  }
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
      size = readVarUInt32(item.data().c_str(), pos);
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

void Parser::onCDemoPacket(const CDemoPacket* packet) {
  dota::bitstream stream(packet->data());
  int type;
  uint32_t size;
  std::vector<pendingMessage> pendingMessages;
  while (stream.end() - stream.position() >= 8) {
    type = stream.nReadUBitVar();
    size = stream.nReadVarUInt32();
    ////std::cout << "type: " << std::to_string(type) << "\n";
    ////std::cout << "size: " << std::to_string(size) << "\n";
    char * buffer = new char[8 * size];
    stream.readBits(buffer, 8 * size);
    pendingMessage msg;
    msg.type = type;
    msg.tick = tick;
    msg.data = std::string(&buffer[0], size);
    delete[] buffer;
    pendingMessages.push_back(msg);
  }
  std::stable_sort(pendingMessages.begin(), pendingMessages.end(), compare_packet_priority);
  for (std::vector<pendingMessage>::iterator it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
    ////std::cout << "priority: " << std::to_string(packet_priority(it->type)) << "\n";
    //std::cout << "TICK: " << std::to_string(tick) << "\n";
    parsePendingMessage(&*it);
  }
}

void Parser::onCDemoFullPacket(const CDemoFullPacket* packet) {
  //parseStringTables(&(packet->string_table()), p);
  onCDemoPacket(&(packet->packet()));
}
