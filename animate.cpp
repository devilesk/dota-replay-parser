#include "parser.hpp"
#include "animate.hpp"
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

SDL_Surface* screen = NULL;
SDL_Surface* background = NULL;
SDL_Surface* herosprites = NULL;
SDL_Surface* itemsprites = NULL;
SDL_Surface* ward_observer = NULL;
SDL_Surface* ward_sentry = NULL;

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
  //p.handle();
  
  std::cout << "Last tick: " << std::to_string(p.tick) << "\n";
  
  init_sprites();
  load_images();
  
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(1536, 952, 32, SDL_SWSURFACE);
 
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
}

bool has_coordinates(PacketEntity* pe) {
 return pe->properties->KV.find("CBodyComponentBaseAnimatingOverlay.m_cellX") != pe->properties->KV.end() &&
        pe->properties->KV.find("CBodyComponentBaseAnimatingOverlay.m_cellY") != pe->properties->KV.end() &&
        pe->properties->KV.find("CBodyComponentBaseAnimatingOverlay.m_vecX") != pe->properties->KV.end() &&
        pe->properties->KV.find("CBodyComponentBaseAnimatingOverlay.m_vecY") != pe->properties->KV.end();
}

void get_coordinates(PacketEntity* pe, int& img_x, int& img_y) {
  uint64_t cX;
  uint64_t cY;
  float vX;
  float vY;
  //std::cout << "CBodyComponentBaseAnimatingOverlay.m_cellX: " << asString(pe->properties->KV["CBodyComponentBaseAnimatingOverlay.m_cellX"]) << "\n";
  //std::cout << "CBodyComponentBaseAnimatingOverlay.m_cellY: " << asString(pe->properties->KV["CBodyComponentBaseAnimatingOverlay.m_cellY"]) << "\n";
  //std::cout << "CBodyComponentBaseAnimatingOverlay.m_vecX: " << asString(pe->properties->KV["CBodyComponentBaseAnimatingOverlay.m_vecX"]) << "\n";
  //std::cout << "CBodyComponentBaseAnimatingOverlay.m_vecY: " << asString(pe->properties->KV["CBodyComponentBaseAnimatingOverlay.m_vecY"]) << "\n";
  pe->properties->fetchUint64("CBodyComponentBaseAnimatingOverlay.m_cellX", cX);
  //std::cout << "cX: " << std::to_string(cX) << "\n";
  pe->properties->fetchUint64("CBodyComponentBaseAnimatingOverlay.m_cellY", cY);
  //std::cout << "cY: " << std::to_string(cY) << "\n";
  pe->properties->fetchFloat32("CBodyComponentBaseAnimatingOverlay.m_vecX", vX);
  //std::cout << "vX: " << std::to_string(vX) << "\n";
  pe->properties->fetchFloat32("CBodyComponentBaseAnimatingOverlay.m_vecY", vY);
  //std::cout << "vY: " << std::to_string(vY) << "\n";
  
  double x = (((double)cX * cellWidth) - MAX_COORDINATE) + (double)vX;
  double y = (((double)cY * cellWidth) - MAX_COORDINATE) + (double)vY;
  //std::cout << "x: " << std::to_string(x) << "\n";
  //std::cout << "y: " << std::to_string(y) << "\n";
  
  img_x = int((8576.0 + x) * 0.0626 + -25.0152);
  img_y = int((8192.0 - y) * 0.0630 + -45.7787);
}

void main_loop() {
  while (p.good() && p.tick < replayTick) {
    p.read();
  }

  
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
  SDL_BlitSurface(background, NULL, screen, NULL);
  
  for(auto& kv: p.packetEntities) {
    if (isPrefix(kv.second->className, "CDOTA_Unit_Hero_")) {
      if (has_coordinates(kv.second)) {
        //std::cout << "entity className: " << kv.second->className << " entity id: " << kv.second->index << "\n";
        
        int img_x;
        int img_y;
        get_coordinates(kv.second, img_x, img_y);
        
        //std::cout << "img_x: " << std::to_string(img_x) << "\n";
        //std::cout << "img_y: " << std::to_string(img_y) << "\n";
        //std::cout << "hero_map[kv.second->className]: " << std::to_string(hero_map[kv.second->className]) << "\n";
        
        SDL_Rect dstrect;
        dstrect.x = img_x - 16;
        dstrect.y = img_y - 16;
        dstrect.w = 32;
        dstrect.h = 32;
        SDL_BlitSurface(herosprites, &(h_offsets[hero_map[kv.second->className]]), screen, &dstrect);
        //std::cout << "drew hero\n";
      }
    } else if (isPrefix(kv.second->className, "CDOTA_BaseNPC_Creep")) {
      if (has_coordinates(kv.second)) {
        uint64_t team;
        kv.second->fetchUint64("m_iTeamNum", team);
        //std::cout << "creep team: " << std::to_string(team) << "\n";
        int img_x;
        int img_y;
        get_coordinates(kv.second, img_x, img_y);
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
    } else if (isPrefix(kv.second->className, "CDOTA_BaseNPC_Fort") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Barracks") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Tower")) {
      if (has_coordinates(kv.second)) {
        uint64_t team;
        kv.second->fetchUint64("m_iTeamNum", team);
        //std::cout << "creep team: " << std::to_string(team) << "\n";
        int img_x;
        int img_y;
        get_coordinates(kv.second, img_x, img_y);
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
    } else if (isPrefix(kv.second->className, "CDOTA_NPC_Observer_Ward")) {
      if (has_coordinates(kv.second)) {
        uint64_t team;
        kv.second->fetchUint64("m_iTeamNum", team);
        //std::cout << "creep team: " << std::to_string(team) << "\n";
        int img_x;
        int img_y;
        get_coordinates(kv.second, img_x, img_y);
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
    }
  }
  /*SDL_Rect dstrect;
  dstrect.x = 1536/2;
  dstrect.y = 952/2;
  dstrect.w = 32;
  dstrect.h = 32;
  SDL_BlitSurface(herosprites, &(h_offsets[sprite_t::CDOTA_Unit_Hero_Luna]), screen, &dstrect);*/
  
  SDL_Flip(screen);
  ++replayTick;
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