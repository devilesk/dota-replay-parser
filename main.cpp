#include "parser.hpp"

Parser p;
std::string CNAME;

int main(int argc, char **argv) {
  if (argc < 3) {
      std::cerr << "Usage: classdump <class name> <replay file>" << std::endl;
      return 1;
  }

  CNAME = std::string(argv[1]);
  std::string path(argv[2]);
  
  p.open(path);
  p.readHeader();
  p.packetEntityHandlers.push_back(entityHandler);
  p.handle();
  
  std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
  
  std::cout << "done" << std::endl;
}


void entityHandler(PacketEntity* pe, EntityEventType t) {
  if (pe->className.compare(CNAME) == 0) {
    std::cout << "entity className: " << pe->className << " " << std::to_string(t) << " entity id: " << pe->index << "\n";
    for (auto const& p : pe->properties->KV)
    {
        std::cout << "\t" << p.first << " " << asString(p.second) << "\n";
    }
    std::cout << "\n";
    for (auto const& p : pe->classBaseline->KV)
    {
        std::cout << "\t" << p.first << " " << asString(p.second) << "\n";
    }
  }
}