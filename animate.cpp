#include "animate.hpp"

SDL_Surface* screen = NULL;
SDL_Surface* background = NULL;
SDL_Surface* herosprites = NULL;
SDL_Surface* itemsprites = NULL;
SDL_Surface* ward_observer = NULL;
SDL_Surface* ward_sentry = NULL;
SDL_Surface* death_x = NULL;
SDL_Surface* courier = NULL;
SDL_Surface* courier_flying = NULL;

TTF_Font* font;

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

  //p.packetEntityHandlers.push_back(entityHandler);
  p.open(path);
  p.generateFullPacketCache();
  p.readHeader();
  //p.skipTo(50322); //14322
  //replayTick = 50322;
  //p.handle();
  
  init_sprites();
  load_images();
  
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(1536, 952, 32, SDL_SWSURFACE);
  
  TTF_Init();
  font = load_font("assets/DejaVuSans.ttf", 12);
 
  #ifdef EMSCRIPTEN
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
    emscripten_set_main_loop(main_loop, 0, 1);
  #else
    while (1) {
      main_loop();
      // Delay to keep frame rate constant (using SDL)
      SDL_Delay(33);
    }
  #endif
  
  SDL_Quit();

  std::cout << "done" << std::endl;
}

TTF_Font* load_font(const char* file, int ptsize) {
    TTF_Font* tmpfont;
    tmpfont = TTF_OpenFont(file, ptsize);
    if (tmpfont == NULL){
        std::cout << "Unable to load font: " << file << TTF_GetError() << std::endl;
    }
    return tmpfont;
}

SDL_Surface* load_image(std::string path) {
  SDL_Surface* img = IMG_Load(path.c_str());
  if (img == NULL) {
      std::cout << "could not load " << path << std::endl;
  }
  else {
    std::cout << "loaded " << path << std::endl;
  }
  return img;
}

void load_images() {
  background = load_image("assets/dota_map.jpg");
  herosprites = load_image("assets/miniheroes.png");
  itemsprites = load_image("assets/items.jpg");
  ward_observer = load_image("assets/ward_observer.png");
  ward_sentry = load_image("assets/ward_sentry.png");
  courier = load_image("assets/courier.png");
  courier_flying = load_image("assets/courier_flying.png");
  death_x = load_image("assets/death_x.png");
}

bool getCoordinates(PacketEntity* pe, int& img_x, int& img_y) {
  uint64_t cX;
  uint64_t cY;
  float vX;
  float vY;
  
  if (!pe->fetchUint64("CBodyComponentBaseAnimatingOverlay.m_cellX", cX)) return false;
  if (!pe->fetchUint64("CBodyComponentBaseAnimatingOverlay.m_cellY", cY)) return false;
  if (!pe->fetchFloat32("CBodyComponentBaseAnimatingOverlay.m_vecX", vX)) return false;
  if (!pe->fetchFloat32("CBodyComponentBaseAnimatingOverlay.m_vecY", vY)) return false;
  
  double x = (((double)cX * cellWidth) - MAX_COORDINATE) + (double)vX;
  double y = (((double)cY * cellWidth) - MAX_COORDINATE) + (double)vY;
  
  img_x = int((8576.0 + x) * 0.0626 + -25.0152);
  img_y = int((8192.0 - y) * 0.0630 + -45.7787);
  return true;
}

void main_loop() {
  while (p.good() && p.tick < replayTick) {
    p.read();
  }

  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
  SDL_BlitSurface(background, NULL, screen, NULL);
  
  for(auto& kv : p.packetEntities) {
    if (isPrefix(kv.second->className, "CDOTA_Unit_Hero_")) {
      //std::cout << "entity className: " << kv.second->className << " entity id: " << kv.second->index << "\n";
      
      int img_x;
      int img_y;
      if (!getCoordinates(kv.second, img_x, img_y)) continue;
      
      //std::cout << "img_x: " << std::to_string(img_x) << "\n";
      //std::cout << "img_y: " << std::to_string(img_y) << "\n";
      //std::cout << "hero_map[kv.second->className]: " << std::to_string(hero_map[kv.second->className]) << "\n";
      
      SDL_Rect dstrect;
      dstrect.x = img_x - 16;
      dstrect.y = img_y - 16;
      dstrect.w = 32;
      dstrect.h = 32;
      SDL_BlitSurface(herosprites, &(h_offsets[hero_map[kv.second->className]]), screen, &dstrect);
      
      int32_t health;
      if (kv.second->fetchInt32("m_iHealth", health)) {
        if (health == 0) {
          dstrect.x = img_x - 10;
          dstrect.y = img_y - 10;
          dstrect.w = 20;
          dstrect.h = 20;
          SDL_BlitSurface(death_x, NULL, screen, &dstrect);
        }
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_Unit_Courier")) {
      bool isFlying;
      kv.second->fetchBool("m_bFlyingCourier", isFlying);
    
      int img_x;
      int img_y;
      if (!getCoordinates(kv.second, img_x, img_y)) continue;
      
      SDL_Rect dstrect;
      
      if (!isFlying) {
        dstrect.x = img_x - 8;
        dstrect.y = img_y - 8;
        dstrect.w = 16;
        dstrect.h = 16;
        SDL_BlitSurface(courier, NULL, screen, &dstrect);
      }
      else {
        dstrect.x = img_x - 15;
        dstrect.y = img_y - 8;
        dstrect.w = 30;
        dstrect.h = 16;
        SDL_BlitSurface(courier_flying, NULL, screen, &dstrect);
      }

      float respawnTime;
      if (kv.second->fetchFloat32("m_flRespawnTime", respawnTime)) {
        if (respawnTime > 0) {
          dstrect.x = img_x - 10;
          dstrect.y = img_y - 10;
          dstrect.w = 20;
          dstrect.h = 20;
          SDL_BlitSurface(death_x, NULL, screen, &dstrect);
        }
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_BaseNPC_Creep")) {
      uint64_t team;
      getTeam(kv.second, team);

      int img_x;
      int img_y;
      if (!getCoordinates(kv.second, img_x, img_y)) continue;
      
      SDL_Rect dstrect;
      dstrect.x = img_x - 2;
      dstrect.y = img_y - 2;
      dstrect.w = 4;
      dstrect.h = 4;
      if (team == 3) {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 255, 0, 0));
      }
      else if (team == 2) {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 0, 255, 0));
      }
      else if (team == 4) {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 255, 127, 0));
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_BaseNPC_Fort") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Barracks") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Tower")) {
      uint64_t team;
      getTeam(kv.second, team);

      int img_x;
      int img_y;
      if (!getCoordinates(kv.second, img_x, img_y)) continue;

      SDL_Rect dstrect;
      dstrect.x = img_x - 4;
      dstrect.y = img_y - 4;
      dstrect.w = 8;
      dstrect.h = 8;
      if (team == 3) {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 255, 0, 0));
      }
      else {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 0, 255, 0));
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_NPC_Observer_Ward")) {
      uint64_t team;
      getTeam(kv.second, team);

      int img_x;
      int img_y;
      if (!getCoordinates(kv.second, img_x, img_y)) continue;

      SDL_Rect dstrect;
      dstrect.x = img_x - 3;
      dstrect.y = img_y - 3;
      dstrect.w = 6;
      dstrect.h = 6;
      if (team == 3) {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 255, 0, 0));
      }
      else {
        SDL_FillRect(screen, &dstrect, SDL_MapRGB(screen->format, 0, 255, 0));
      }
      
      dstrect.x = img_x - 8;
      dstrect.y = img_y - 23;
      dstrect.w = 16;
      dstrect.h = 24;
      if (kv.second->className.compare("CDOTA_NPC_Observer_Ward") == 0) {
        SDL_BlitSurface(ward_observer, NULL, screen, &dstrect);
      }
      else if (kv.second->className.compare("CDOTA_NPC_Observer_Ward_TrueSight") == 0) {
        SDL_BlitSurface(ward_sentry, NULL, screen, &dstrect);
      }
    }
    else if (isPrefix(kv.second->className, "CDOTAGamerulesProxy")) {
    
    }
    else if (isPrefix(kv.second->className, "CDOTA_DataSpectator")) {
    
    }
    else if (isPrefix(kv.second->className, "CDOTA_DataDire")) {
    
    }
    else if (isPrefix(kv.second->className, "CDOTA_DataRadiant")) {
    
    }
    else if (isPrefix(kv.second->className, "CDOTA_PlayerResource")) {
      /*for (int i = 0; i < 10; ++i) {
        std::string n = "000" + std::to_string(i);
        std::string playerName;
        uint64_t steamId;
        uint32_t hero;
        if (kv.second->fetchString("m_vecPlayerData.0000.m_iszPlayerName", playerName)) {
          std::cout << "player: " << playerName << "\n";
        }
        if (kv.second->fetchUint64("m_vecPlayerData.0004.m_iPlayerSteamID", steamId)) {
          std::cout << " steamId: " << std::to_string(steamId) << "\n";
        }
        if (kv.second->fetchUint32("m_vecPlayerTeamData.0009.m_hSelectedHero", hero)) {
          std::cout << " hero: " << std::to_string(hero) << "\n";
        }
      }*/
      std::string playerName;
      uint64_t steamId;
      uint32_t hero;
      if (kv.second->fetchString("m_vecPlayerData.0000.m_iszPlayerName", playerName)) {
        std::cout << "player: " << playerName << "\n";
      }
      if (kv.second->fetchUint64("m_vecPlayerData.0004.m_iPlayerSteamID", steamId)) {
        std::cout << " steamId: " << std::to_string(steamId) << "\n";
      }
      if (kv.second->fetchUint32("m_vecPlayerTeamData.0009.m_hSelectedHero", hero)) {
        std::cout << " hero: " << std::to_string(hero) << "\n";
      }
    }
  }

  SDL_Flip(screen);
  ++replayTick;
}