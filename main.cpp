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

  //p.open("1858267282.dem");
  p.open(path);
  p.readHeader();
  p.skipTo(50322);
  replayTick = 14322;
  p.packetEntityHandlers.push_back(entityHandler);
  p.handle();
  
  std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
  
  std::cout << "done" << std::endl;
}


void entityHandler(PacketEntity* pe, EntityEventType t) {
  //std::cout << "entity className: " << pe->className << " " << std::to_string(t) << " entity id: " << pe->index << "\n";
  if (pe->className.compare("CDOTA_PlayerResource") == 0) {
    std::cout << "entity className: " << pe->className << " " << std::to_string(t) << " entity id: " << pe->index << "\n";
    /*for (int i = 0; i < 10; ++i) {
      std::string n = "000" + std::to_string(i);
      std::string playerName;
      uint64_t steamId;
      uint32_t hero;
      pe->fetchString("m_vecPlayerData." + n + ".m_iszPlayerName", playerName);
      pe->fetchUint64("m_vecPlayerData." + n + ".m_iPlayerSteamID", steamId);
      pe->fetchUint32("m_vecPlayerTeamData." + n + ".m_hSelectedHero", hero);
      std::cout << "player: " << playerName << " hero: " << std::to_string(hero) << " steamId: " << std::to_string(steamId) << "\n";
      std::cout << asString(pe->classBaseline->KV["m_vecPlayerData.0000.m_iszPlayerName"]) << "\n";
      std::cout << asString(pe->classBaseline->KV["m_vecPlayerData.0000.m_iPlayerSteamID"]) << "\n";
      std::cout << asString(pe->properties->KV["m_vecPlayerTeamData.0000.m_hSelectedHero"]) << "\n";
    }*/
    std::cout << asString(pe->classBaseline->KV["m_vecPlayerData.0000.m_iszPlayerName"]) << "\n";
    std::cout << asString(pe->classBaseline->KV["m_vecPlayerData.0000.m_iPlayerSteamID"]) << "\n";
    std::cout << asString(pe->properties->KV["m_vecPlayerTeamData.0000.m_hSelectedHero"]) << "\n";
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