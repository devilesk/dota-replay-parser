#include "snappy.h"
#include <iostream>
#include <string>

int main ()
{
  std::string str = "Hello";
  const char* c = str.c_str();
  std::string output = "";
  std::string uncompressed_output = "";
  snappy::Compress(c, 5, &output);
  const char* d = output.c_str();
  snappy::Uncompress(d, output.size(), &uncompressed_output);
  std::cout << uncompressed_output << "\n";
}
