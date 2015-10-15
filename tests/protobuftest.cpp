#include <snappy.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// Protobuf objects
#include "../protobufs/ai_activity.pb.h"
#include "../protobufs/demo.pb.h"
#include "../protobufs/dota_commonmessages.pb.h"
#include "../protobufs/dota_modifiers.pb.h"
#include "../protobufs/usermessages.pb.h"
#include "../protobufs/netmessages.pb.h"
#include "../protobufs/networkbasetypes.pb.h"
#include "../protobufs/usermessages.pb.h"

uint32_t readVarUInt32(std::ifstream &stream);
uint32_t readMessage(std::ifstream &stream);

int main ()
{
  char* buffer;
  std::ifstream stream;
  std::string path = "1781962623_source2.dem";
  // std::ifstream::in and std::ifstream::binary are mode flags for input and binary
  stream.open(path.c_str(), std::ifstream::in | std::ifstream::binary);
  if (!stream.is_open()) {
    std::cout << "Could not open file.\n";
  }
  else {
    std::cout << "File opened.\n";
  }

  // get file length
  stream.seekg(0, stream.end);
  int length = stream.tellg();
  stream.seekg(0, stream.beg);

  buffer = new char[1];
  char* header = new char[8];

  stream.read(header, 8);
  std::string demheader = "PBDEMS2";
  std::cout << std::to_string(sizeof(header)) << "\n";
  std::cout << header << "\n";

  if (strcmp(header, demheader.c_str())) {
    std::cout << "Invalid header.\n";
  }
  else {
    std::cout << "Valid header.\n";
  }

  delete buffer;
  delete header;

  // Skip the next 8 bytes, which appear to be two int32s related to the size
  // of the demo file. We may need them in the future, but not so far.
  stream.seekg(8, stream.cur);

  uint32_t cmd;
  cmd = readVarUInt32(stream);
  std::cout << "command: " << std::to_string(cmd) << "\n";

  uint32_t tick;
  tick = readVarUInt32(stream);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(stream);
  std::cout << "size: " << std::to_string(size) << "\n";

  buffer = new char[size];
  stream.read(buffer, size);

  delete buffer;

  while (stream.good()) {
    readMessage(stream);
  }

  std::cout << "File length: " << std::to_string(length) << "\n";
  stream.close();
  std::cout << "File closed.\n";
}

uint32_t readMessage(std::ifstream &stream) {
  std::cout << "readMessage\n";
  char* buffer;
  uint32_t cmd;
  cmd = readVarUInt32(stream);
  const bool compressed = cmd & 112;
  cmd = (cmd & ~112);
  std::cout << "command: " << std::to_string(cmd) << " compressed: " << std::to_string(compressed) << "\n";

  uint32_t tick;
  tick = readVarUInt32(stream);

  // This appears to actually be an int32, where a -1 means pre-game.
  if (tick == 4294967295) {
    tick = 0;
  }

  std::cout << "tick: " << std::to_string(tick) << "\n";

  uint32_t size;
  size = readVarUInt32(stream);
  std::cout << "size: " << std::to_string(size) << "\n";

  buffer = new char[size];
  stream.read(buffer, size);
  std::cout << "size2: " << std::to_string(size) << "\n";

  if (compressed && snappy::IsValidCompressedBuffer(buffer, size)) {
    std::cout << "size3: " << std::to_string(size) << "\n";

    std::cout << "valid snappy compressed buffer\n";
  }
  std::cout << "size4: " << std::to_string(size) << "\n";

  delete buffer;
  std::cout << "size5: " << std::to_string(size) << "\n";
  return 0;
}

uint32_t readVarUInt32(std::ifstream &stream) {
  uint32_t i = 0;
  uint32_t value = 0;
  unsigned char b;
  do {
    stream.read((char*)&b, 1);
    value |= ((int)b & 0x7F) << (7 * i);
    ++i;
  } while (b & 0x80 && i < 35);
  return value;
}
