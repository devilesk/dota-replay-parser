#include "parser.hpp"

Parser p;
int replayTick = 0;
const double cellWidth = (double)(1 << 7);
const double MAX_COORDINATE = 16384;

int main(int argc, char **argv) {
  /*if (argc < 2) {
      std::cerr << "Usage: example <file>" << std::endl;
      return 1;
  }

  std::string path(argv[1]);*/
  std::string path("1858267282.dem");

  p.packetEntityHandlers.push_back(entityHandler);
  //p.open("1858267282.dem");
  p.open(path);
  p.readHeader();
  p.skipTo(2000);
  replayTick = 14322;
  //p.handle();
  
  std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
  
  std::cout << "done" << std::endl;
}


void entityHandler(PacketEntity* pe, EntityEventType t) {
  //std::cout << "entity className: " << pe->className << " " << std::to_string(t) << " entity id: " << pe->index << "\n";
  if (pe->className.compare("CDOTA_NPC_Observer_Ward") == 0 || isPrefix(pe->className, "CDOTA_Unit_Hero_")) {
    //std::cout << "entity className: " << pe->className << " " << std::to_string(t) << " entity id: " << pe->index << "\n";
    for (auto const& p : pe->properties->KV)
    {
        //std::cout << "\t" << p.first << " " << asString(p.second) << "\n";
    }
  }
}