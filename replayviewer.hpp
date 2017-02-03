#ifndef _REPLAYVIEWER_HPP_
#define _REPLAYVIEWER_HPP_

#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include "parser.hpp"

struct Point2f {
  float x;
  float y;
};

struct HeroState {
  bool isIllusion;
  Point2f pos;
  std::string className;
  int lifeState;
  int health;
};

struct CreepState {
  bool isWaitingToSpawn;
  int lifeState;
  int team;
  Point2f pos;
};

struct WardState {
  int team;
  std::string className;
  Point2f pos;
};

struct CourierState {
  int lifeState;
  bool isFlying;
  Point2f pos;
  float respawnTime;
};

struct BuildingState {
  int team;
  std::string className;
  Point2f pos;
  int health;
  int maxHealth;
};

struct ItemState {
  std::string name;
  bool isEmpty;
};

struct AbilityState {
  std::string name;
  int level;
  float cooldown;
};

struct PlayerResourceState {
  std::string playerName;
  std::string heroClassName;
  float steamId;
  int heroEntId;
  int kills;
  int deaths;
  int assists;
  int streak;
  int level;
  int respawnSeconds;
  std::vector<AbilityState> abilities;
  std::vector<ItemState> items;
};

struct GameState {
  bool isPaused;
  int state;
  float time;
  float startTime;
  float preGameStartTime;
  float stateTransitionTime;
  float extraTimeRadiant;
  float extraTimeDire;
  int netTimeOfDay;
  int heroPickState;
  int activeTeam;
  int startingTeam;
  std::vector<int> bans;
  std::vector<int> picks;
};

struct TeamState {
  std::vector<int> reliableGold;
  std::vector<int> unreliableGold;
};

struct TickState {
  int tick;
  std::vector<HeroState> heroes;
  std::vector<CreepState> creeps;
  std::vector<WardState> wards;
  std::vector<CourierState> couriers;
  std::vector<BuildingState> buildings;
  std::vector<PlayerResourceState> playerResources;
  GameState game;
  TeamState dire;
  TeamState radiant;
};

class ReplayViewer {
  private:
    Parser p;
    int replayTick;
    const double cellWidth = (double)(1 << 7);
    const double MAX_COORDINATE = 16384;
    int minuteTick;
    int gameState;
    bool isPaused;
    int playbackSpeed;
    float gameTime;
  public:

    
    ReplayViewer() {
      replayTick = 0;
      minuteTick = -1;
      isPaused = false;
      playbackSpeed = 1;
    };
    
    int getReplayTick() const { return replayTick; };
    void setReplayTick(int _replayTick) { replayTick = _replayTick; };
    
    void open(std::string path);
    
    bool good() { return p.good(); };
    
    int getStopTick() { return (int)p.stopTick; };
    
    void setTick(int tick);
    
    int changePlayback(int state);
    int skipToNextFullPacket(int direction);
    TickState getCurrentTickState();
    
    bool has_coordinates(PacketEntity* pe);
    
    bool getCoordinates(PacketEntity* pe, Point2f& pos);
    
    inline bool getMaxHealth(PacketEntity* pe, uint64_t& maxHealth) {
      return !pe->fetchUint64("m_iMaxHealth", maxHealth);
    }
    inline bool getTeam(PacketEntity* pe, uint64_t& team) {
      return !pe->fetchUint64("m_iTeamNum", team);
    }
};

#endif /* _REPLAYVIEWER_HPP_ */