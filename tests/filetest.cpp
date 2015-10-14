#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

int main ()
{
  std::ifstream stream;
  std::string path = "testfiles/1781962623_source2.dem";
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

  char* buffer = new char[1];

  char * header = new char[8];
  stream.read(header, sizeof(header));
  std::string demheader = "PBDEMS2";
  std::cout << std::to_string(sizeof(header)) << "\n";
  std::cout << header << "\n";

  if (strcmp(header, demheader.c_str())) {
    std::cout << "Invalid header.\n";
  }
  else {
    std::cout << "Valid header.\n";
  }

  while (stream.good()) {
    stream.read(buffer, 1);
  }

  std::cout << "File length: " << std::to_string(length) << "\n";
  stream.close();
  std::cout << "File closed.\n";
}
