#include <emscripten/bind.h>
#include "replayviewer.hpp"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(stl_wrappers) {
    register_vector<int>("VectorInt");
    register_vector<HeroState>("VectorHeroState");
    register_vector<CreepState>("VectorCreepState");
    register_vector<WardState>("VectorWardState");
    register_vector<CourierState>("VectorCourierState");
    register_vector<BuildingState>("VectorBuildingState");
    register_vector<PlayerResourceState>("VectorPlayerResourceState");
    register_vector<AbilityState>("VectorAbilityState");
    register_vector<ItemState>("VectorItemState");
}

EMSCRIPTEN_BINDINGS(ReplayViewer) {
    class_<ReplayViewer>("ReplayViewer")
        .constructor()
        .function("open", &ReplayViewer::open)
        .function("good", &ReplayViewer::good)
        .function("handle", &ReplayViewer::handle)
        .function("setTick", &ReplayViewer::setTick)
        .function("getStopTick", &ReplayViewer::getStopTick)
        .function("skipToNextFullPacket", &ReplayViewer::skipToNextFullPacket)
        .function("getCurrentTickState", &ReplayViewer::getCurrentTickState)
        .function("changePlayback", &ReplayViewer::changePlayback)
        .property("replayTick", &ReplayViewer::getReplayTick, &ReplayViewer::setReplayTick)
        ;
            
    value_array<Point2f>("Point2f")
        .element(&Point2f::x)
        .element(&Point2f::y)
        ;
        
    value_object<HeroState>("HeroState")
        .field("entId", &HeroState::entId)
        .field("isIllusion", &HeroState::isIllusion)
        .field("pos", &HeroState::pos)
        .field("className", &HeroState::className)
        .field("lifeState", &HeroState::lifeState)
        .field("health", &HeroState::health)
        ;
            
    value_object<CreepState>("CreepState")
        .field("entId", &CreepState::entId)
        .field("isWaitingToSpawn", &CreepState::isWaitingToSpawn)
        .field("lifeState", &CreepState::lifeState)
        .field("team", &CreepState::team)
        .field("pos", &CreepState::pos)
        ;
            
    value_object<WardState>("WardState")
        .field("entId", &WardState::entId)
        .field("team", &WardState::team)
        .field("className", &WardState::className)
        .field("pos", &WardState::pos)
        ;
        
    value_object<CourierState>("CourierState")
        .field("entId", &CourierState::entId)
        .field("lifeState", &CourierState::lifeState)
        .field("isFlying", &CourierState::isFlying)
        .field("pos", &CourierState::pos)
        .field("respawnTime", &CourierState::respawnTime)
        ;
        
    value_object<BuildingState>("BuildingState")
        .field("entId", &BuildingState::entId)
        .field("team", &BuildingState::team)
        .field("className", &BuildingState::className)
        .field("pos", &BuildingState::pos)
        .field("health", &BuildingState::health)
        .field("maxHealth", &BuildingState::maxHealth)
        ;
        
    value_object<ItemState>("ItemState")
        .field("entId", &ItemState::entId)
        .field("name", &ItemState::name)
        .field("isEmpty", &ItemState::isEmpty)
        ;
            
    value_object<AbilityState>("AbilityState")
        .field("entId", &AbilityState::entId)
        .field("name", &AbilityState::name)
        .field("level", &AbilityState::level)
        .field("cooldown", &AbilityState::cooldown)
        ;
        
    value_object<PlayerResourceState>("PlayerResourceState")
        .field("playerName", &PlayerResourceState::playerName)
        .field("heroClassName", &PlayerResourceState::heroClassName)
        .field("steamId", &PlayerResourceState::steamId)
        .field("heroEntId", &PlayerResourceState::heroEntId)
        .field("kills", &PlayerResourceState::kills)
        .field("deaths", &PlayerResourceState::deaths)
        .field("assists", &PlayerResourceState::assists)
        .field("streak", &PlayerResourceState::streak)
        .field("level", &PlayerResourceState::level)
        .field("respawnSeconds", &PlayerResourceState::respawnSeconds)
        .field("abilities", &PlayerResourceState::abilities)
        .field("items", &PlayerResourceState::items)
        ;
            
    value_object<GameState>("GameState")
        .field("entId", &GameState::entId)
        .field("isPaused", &GameState::isPaused)
        .field("state", &GameState::state)
        .field("time", &GameState::time)
        .field("startTime", &GameState::startTime)
        .field("preGameStartTime", &GameState::preGameStartTime)
        .field("stateTransitionTime", &GameState::stateTransitionTime)
        .field("extraTimeRadiant", &GameState::extraTimeRadiant)
        .field("extraTimeDire", &GameState::extraTimeDire)
        .field("netTimeOfDay", &GameState::netTimeOfDay)
        .field("heroPickState", &GameState::heroPickState)
        .field("activeTeam", &GameState::activeTeam)
        .field("startingTeam", &GameState::startingTeam)
        .field("bans", &GameState::bans)
        .field("picks", &GameState::picks)
        ;
            
    value_object<TeamState>("TeamState")
        .field("entId", &TeamState::entId)
        .field("reliableGold", &TeamState::reliableGold)
        .field("unreliableGold", &TeamState::unreliableGold)
        ;
    
    value_object<TickState>("TickState")
        .field("tick", &TickState::tick)
        .field("heroes", &TickState::heroes)
        .field("creeps", &TickState::creeps)
        .field("wards", &TickState::wards)
        .field("couriers", &TickState::couriers)
        .field("buildings", &TickState::buildings)
        .field("playerResources", &TickState::playerResources)
        .field("game", &TickState::game)
        .field("dire", &TickState::dire)
        .field("radiant", &TickState::radiant)
        ;
}