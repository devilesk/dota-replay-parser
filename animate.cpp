#include "animate.hpp"

SDL_Surface* screen = NULL;
SDL_Surface* background = NULL;
SDL_Surface* herosprites = NULL;
SDL_Surface* illusionsprites = NULL;
SDL_Surface* itemsprites = NULL;
SDL_Surface* ward_observer = NULL;
SDL_Surface* ward_sentry = NULL;
SDL_Surface* death_x = NULL;
SDL_Surface* courier = NULL;
SDL_Surface* courier_flying = NULL;

SDL_Surface* draft_background = NULL;
SDL_Surface* ban_stroke = NULL;

ImageSet portrait_icons("heroes74x42/", ".png", on_portrait_icon_load, onerror);
ImageSet small_portrait_icons("heroes42x23/", ".png", on_small_portrait_icon_load, onerror);
ImageSet vert_portrait_icons("vertheroes165x218/", ".png", on_vert_portrait_icon_load, onerror);
std::unordered_map<std::string, SDL_Surface*> ability_icons;
std::unordered_set<std::string> ability_icons_requested;

TTF_Font* font;
TTF_Font* font16;
TTF_Font* font_lucida10;

Parser p;
int replayTick = 0;
const double cellWidth = (double)(1 << 7);
const double MAX_COORDINATE = 16384;
int minuteTick = -1;
int gameState;
std::unordered_map<std::string, uint32_t> colors;
std::unordered_map<uint64_t, uint32_t> team_colors;
bool isPaused = false;
int playbackSpeed = 1;
float gameTime;

extern "C" {
  int setTick(int tick) {
    p.skipTo(tick);
    replayTick = tick;
    isPaused = false;
    return 0;
  }
  int changePlayback(int state) {
    switch (state) {
      case 0:
        isPaused = false;
      break;
      case 1:
        isPaused = true;
      break;
      case 2:
        return (int)skipToNextFullPacket(-1);
      break;
      case 3:
        return (int)skipToNextFullPacket(1);
      break;
      case 4:
        if (playbackSpeed > 1) playbackSpeed = (int)(playbackSpeed / 2);
        return playbackSpeed;
      break;
      case 5:
        if (playbackSpeed < 1024) playbackSpeed = (int)(playbackSpeed * 2);
        return playbackSpeed;
      break;
    }
    return 0;
  }
}

uint32_t skipToNextFullPacket(int direction) {
  uint32_t tick = 0;
  for (int i = 0; i < p.fpackcachetick.size(); ++i) {
    if (direction == -1) {
      if (p.fpackcachetick[i] >= p.tick) {
        break;
      }
      tick = p.fpackcachetick[i];
    }
    else {
      tick = p.fpackcachetick[i];
      if (p.fpackcachetick[i] > p.tick) {
        break;
      }
    }
  }
  p.skipTo(tick);
  replayTick = tick;
  isPaused = false;
  return tick;
}

int main(int argc, char **argv) {
  if (argc < 2) {
      std::cerr << "Usage: program <file>" << std::endl;
      return 1;
  }

  std::string path(argv[1]);
  //std::string path("1858267282.dem");

  //p.packetEntityHandlers.push_back(entityHandler);
  p.open(path);
  p.generateFullPacketCache();
  p.readHeader();
  //p.skipTo(50322); //14322
  //replayTick = 50322;
  //p.handle();
  
  EM_ASM_({
    initSlider($0, $1);
  }, p.stopTick, p.tick);
  
  init_sprites();
  load_images();
  
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(1536, 952, 32, SDL_SWSURFACE);
  
  init_color_map();
  
  TTF_Init();
  font = load_font("Arial", 12);
  font16 = load_font("Arial", 16);
  font_lucida10 = load_font("Lucida Console", 10);
 
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

void init_color_map() {
  colors["green"] = team_colors[2] = SDL_MapRGB(screen->format, 0, 255, 0);
  colors["red"] = team_colors[3] = SDL_MapRGB(screen->format, 255, 0, 0);
  colors["orange"] = team_colors[4] = SDL_MapRGB(screen->format, 255, 127, 0);
}

TTF_Font* load_font(const char* file, int ptsize) {
    TTF_Font* tmpfont;
    tmpfont = TTF_OpenFont(file, ptsize);
    if (tmpfont == NULL){
        std::cout << "Unable to load font: " << file << TTF_GetError() << std::endl;
    }
    return tmpfont;
}

SDL_Surface* draw_text(TTF_Font *fonttodraw, uint8_t fgR, uint8_t fgG, uint8_t fgB, uint8_t fgA, 
                      uint8_t bgR, uint8_t bgG, uint8_t bgB, uint8_t bgA, std::string text, textquality quality) {
                      
    SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
    SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
    SDL_Surface *resulting_text = NULL;

    if (quality == solid) resulting_text = TTF_RenderText_Solid(fonttodraw, text.c_str(), tmpfontcolor);
    else if (quality == shaded) resulting_text = TTF_RenderText_Shaded(fonttodraw, text.c_str(), tmpfontcolor, tmpfontbgcolor);
    else if (quality == blended) resulting_text = TTF_RenderText_Blended(fonttodraw, text.c_str(), tmpfontcolor);

    return resulting_text;
}

SDL_Surface* draw_text(TTF_Font *fonttodraw, const SDL_PixelFormat* format, uint32_t fg, uint8_t fgA, 
                      uint32_t bg, uint8_t bgA, std::string text, textquality quality) {
                      
    uint8_t fgR, fgG, fgB, bgR, bgG, bgB;
    SDL_GetRGB(fg, format, &fgR, &fgG, &fgB);
    SDL_GetRGB(bg, format, &bgR, &bgG, &bgB);

    return draw_text(fonttodraw, fgR, fgG, fgB, fgA, bgR, bgG, bgB, bgA, text, quality);
}

SDL_Surface* draw_text(TTF_Font *fonttodraw, const SDL_PixelFormat* format, uint32_t fg, uint8_t fgA, 
                      uint8_t bgR, uint8_t bgG, uint8_t bgB, uint8_t bgA, std::string text, textquality quality) {
                      
    uint8_t fgR, fgG, fgB;
    SDL_GetRGB(fg, format, &fgR, &fgG, &fgB);

    return draw_text(fonttodraw, fgR, fgG, fgB, fgA, bgR, bgG, bgB, bgA, text, quality);
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

void on_small_portrait_icon_load(const char* filename) {
  std::string portraitName = std::string(filename);
  portraitName = portraitName.substr(0,portraitName.length() - small_portrait_icons.image_ext.length());
  SDL_Surface* portraitIcon = IMG_Load(filename);
  if (portraitIcon == NULL) {
      std::cout << "could not load " << std::string(filename) << std::endl;
  }
  else {
    small_portrait_icons.images[portraitName] = portraitIcon;
    std::cout << "loaded " << std::string(filename) << std::endl;
  }
}


void on_portrait_icon_load(const char* filename) {
  std::string portraitName = std::string(filename);
  portraitName = portraitName.substr(0,portraitName.length() - portrait_icons.image_ext.length());
  SDL_Surface* portraitIcon = IMG_Load(filename);
  if (portraitIcon == NULL) {
      std::cout << "could not load " << std::string(filename) << std::endl;
  }
  else {
    portrait_icons.images[portraitName] = portraitIcon;
    std::cout << "loaded " << std::string(filename) << std::endl;
  }
}

void on_vert_portrait_icon_load(const char* filename) {
  std::string portraitName = std::string(filename);
  portraitName = portraitName.substr(0,portraitName.length() - vert_portrait_icons.image_ext.length());
  SDL_Surface* portraitIcon = IMG_Load(filename);
  if (portraitIcon == NULL) {
      std::cout << "could not load " << std::string(filename) << std::endl;
  }
  else {
    vert_portrait_icons.images[portraitName] = portraitIcon;
    std::cout << "loaded " << std::string(filename) << std::endl;
  }
}

void on_ability_icon_load(const char* filename) {
  std::string abilityName = std::string(filename);
  abilityName = abilityName.substr(0,abilityName.length() - 4);
  SDL_Surface* abilityIcon = IMG_Load(filename);
  if (abilityIcon == NULL) {
      std::cout << "could not load " << std::string(filename) << std::endl;
  }
  else {
    ability_icons[abilityName] = abilityIcon;
    std::cout << "loaded " << std::string(filename) << std::endl;
  }
}

void onerror(const char* filename) {
  std::cout << "emscripten_async_wget onerror " << std::string(filename) << std::endl;
}

void load_images() {
  background = load_image("assets/dota_map.jpg");
  herosprites = load_image("assets/miniheroes.png");
  illusionsprites = load_image("assets/illusions.png");
  itemsprites = load_image("assets/items.jpg");
  ward_observer = load_image("assets/ward_observer.png");
  ward_sentry = load_image("assets/ward_sentry.png");
  courier = load_image("assets/courier.png");
  courier_flying = load_image("assets/courier_flying.png");
  death_x = load_image("assets/death_x.png");
  
  draft_background = load_image("assets/draft/draft_background.png");
  ban_stroke = load_image("assets/draft/ban_stroke.png");
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
  if (isPaused) return;
  
  while (p.good() && p.tick < replayTick) {
    p.read();
  }

  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
  if (gameState >= 4) {
    SDL_BlitSurface(background, NULL, screen, NULL);
  }
  else {
    SDL_BlitSurface(draft_background, NULL, screen, NULL);
  }
  
  for(auto& kv : p.packetEntities) {
    if (isPrefix(kv.second->className, "CDOTA_Unit_Hero_")) {
      //std::cout << "entity className: " << kv.second->className << " entity id: " << kv.second->index << "\n";
      uint64_t lifeState;
      if (!kv.second->fetchUint64("m_lifeState", lifeState)) continue;
      if (lifeState != 0) continue;
      
      uint32_t replicatingOtherHeroModel;
      if (!kv.second->fetchUint32("m_hReplicatingOtherHeroModel", replicatingOtherHeroModel)) continue;
      bool isIllusion = (replicatingOtherHeroModel & 2047) != 2047;
      
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
      if (isIllusion) {
        SDL_BlitSurface(illusionsprites, &(h_offsets[hero_map[kv.second->className]]), screen, &dstrect);
      }
      else {
        SDL_BlitSurface(herosprites, &(h_offsets[hero_map[kv.second->className]]), screen, &dstrect);
      }
      
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
      uint64_t lifeState;
      if (!kv.second->fetchUint64("m_lifeState", lifeState)) continue;
      if (lifeState != 0) continue;
      
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
      bool isWaitingToSpawn;
      if (!kv.second->fetchBool("m_bIsWaitingToSpawn", isWaitingToSpawn)) continue;
      if (isWaitingToSpawn) continue;
      
      uint64_t lifeState;
      if (!kv.second->fetchUint64("m_lifeState", lifeState)) continue;
      if (lifeState != 0) continue;
      
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
      SDL_FillRect(screen, &dstrect, team_colors[team]);
    }
    else if (isPrefix(kv.second->className, "CDOTA_BaseNPC_Fort") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Barracks") ||
               isPrefix(kv.second->className, "CDOTA_BaseNPC_Tower")) {
      if (gameState < 4) continue;
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
      SDL_FillRect(screen, &dstrect, team_colors[team]);
      
      // draw hp
      int32_t health;
      int32_t maxHealth;
      if (!kv.second->fetchInt32("m_iHealth", health)) continue;
      if (!kv.second->fetchInt32("m_iMaxHealth", maxHealth)) continue;
      
      std::string sHealth = std::to_string(health);
      int text_width;
      int text_height;
      TTF_SizeText(font, sHealth.c_str(), &text_width, &text_height);
      dstrect.x = img_x - (text_width / 2);
      dstrect.y = img_y - (4 + text_height);
      dstrect.w = text_width;
      dstrect.h = text_height;
      SDL_Surface* text_surface;
      text_surface = draw_text(font, screen->format, team_colors[team], 255, 0, 0, 0, 0, sHealth, solid);
      SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
      SDL_FreeSurface(text_surface);
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
        SDL_FillRect(screen, &dstrect, colors["red"]);
      }
      else {
        SDL_FillRect(screen, &dstrect, colors["green"]);
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
      float _gameTime;            // current time
      float gameStartTime;        // game time at which the pre-game to game transition occurs (creeps spawning). 
      float preGameStartTime;     // game time when the heroes are loaded in and the pre-game begins.
      float stateTransitionTime;  // game time when game mode changes.
      float extraTimeRadiant;     // seconds of reserve/bonus time remaining (0000 for Radiant, 0001 for Dire)
      float extraTimeDire;        // seconds of reserve/bonus time remaining (0000 for Radiant, 0001 for Dire)
      int32_t gameMode;           // Game Mode
      int32_t _gameState;         // Game State
      int32_t netTimeOfDay;       // Time of Day
      uint32_t heroPickState;     // Hero Pick state
      int32_t activeTeam;         // Active Team   
                                  // 2: Radiant is currently picking/banning.
                                  // 3: Dire is currently picking/banning.
      int32_t startingTeam;       // Starting Team
                                  // 0: Not a CM or reverse-CM game.
                                  // 2: Radiant has first pick.
                                  // 3: Dire has first pick.

      kv.second->fetchFloat32("CDOTAGamerules.m_fGameTime", _gameTime);
      kv.second->fetchFloat32("CDOTAGamerules.m_flGameStartTime", gameStartTime);
      kv.second->fetchFloat32("CDOTAGamerules.m_flPreGameStartTime", preGameStartTime);
      kv.second->fetchFloat32("CDOTAGamerules.m_flStateTransitionTime", stateTransitionTime);
      kv.second->fetchFloat32("CDOTAGamerules.m_fExtraTimeRemaining.0000", extraTimeRadiant);
      kv.second->fetchFloat32("CDOTAGamerules.m_fExtraTimeRemaining.0001", extraTimeDire);
      kv.second->fetchInt32("CDOTAGamerules.m_iGameMode", gameMode);
      kv.second->fetchInt32("CDOTAGamerules.m_nGameState", _gameState);
      kv.second->fetchInt32("CDOTAGamerules.m_iNetTimeOfDay", netTimeOfDay);
      kv.second->fetchUint32("CDOTAGamerules.m_nHeroPickState", heroPickState);
      kv.second->fetchInt32("CDOTAGamerules.m_iActiveTeam", activeTeam);
      kv.second->fetchInt32("CDOTAGamerules.m_iStartingTeam", startingTeam);

      gameTime = _gameTime;
      
      int clock = 0;
      if (gameState == 4) {
        clock = int(stateTransitionTime - gameTime);
      }
      else if (gameState > 4) {
        clock = int(gameTime-gameStartTime);
        int mins = std::floor(clock / 60);
        if (((int)clock) % 60 == 0 && mins > minuteTick) {
          minuteTick = mins;
        }
      }
      
      // create clock string
      {
        char clock_str[6];
        int mins = std::floor(clock / 60);
        int secs = ((int)clock) % 60;
        snprintf(clock_str, 6, "%02d:%02d", mins, secs);
        clock_str[5] = '\0';
        
        // draw game time
        SDL_Rect dstrect;
        dstrect.x = 5;
        dstrect.y = 5;
        dstrect.w = 64;
        dstrect.h = 16;
        SDL_Surface* text_surface = draw_text(font, 255, 255, 255, 255, 0, 0, 0, 0, clock_str, solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
      }
      
      if (gameState != _gameState) {
        gameState = (int)_gameState;
        std::cout << "stateChange" << ", " << std::to_string(gameState) << ", " << std::to_string(clock) << ", " << std::to_string(p.tick) << std::endl;
      }
    
      if (gameState >= 4) continue;
      for (int i = 0; i < 10; ++i) {
        std::string n = "000" + std::to_string(i);
        int32_t bannedHero;
        int32_t pickedHero;
        kv.second->fetchInt32("CDOTAGamerules.m_BannedHeroes." + n, bannedHero);
        kv.second->fetchInt32("CDOTAGamerules.m_SelectedHeroes." + n, pickedHero);

        if (bannedHero != 0) {
          SDL_Surface* portrait_icon = portrait_icons.get_image(heroid_map[bannedHero]);
          if (portrait_icon != nullptr) {
            SDL_Rect dstrect;
            dstrect.x = 124;
            dstrect.y = 576 + (i % 5) * 47;
            dstrect.w = 74;
            dstrect.h = 42;
            if (i >= 5) dstrect.y -= 353;
            SDL_BlitSurface(portrait_icon, NULL, screen, &dstrect);
            
            dstrect.x -= 8;
            dstrect.y += 2;
            dstrect.w = 86;
            dstrect.h = 39;
            SDL_BlitSurface(ban_stroke, NULL, screen, &dstrect);
          }
        }
        if (pickedHero != 0) {
          SDL_Surface* vert_portrait_icon = vert_portrait_icons.get_image(heroid_map[pickedHero]);
          if (vert_portrait_icon != nullptr) {
            SDL_Rect dstrect;
            dstrect.x = 261 + (i % 5) * 201;
            dstrect.y = 574;
            dstrect.w = 165;
            dstrect.h = 228;
            if (i >= 5) dstrect.y -= 350;
            SDL_BlitSurface(vert_portrait_icon, NULL, screen, &dstrect);
          }
        }
      }
      
      // radiant reserve time
      {
        char clock_str[6];
        int mins = std::floor(extraTimeRadiant / 60);
        int secs = ((int)extraTimeRadiant) % 60;
        snprintf(clock_str, 6, "%02d:%02d", mins, secs);
        clock_str[5] = '\0';

        // draw extra remaining time
        SDL_Rect dstrect;
        dstrect.x = 137;
        dstrect.y = 546;
        dstrect.w = 64;
        dstrect.h = 16;
        SDL_Surface* text_surface = draw_text(font16, screen->format, colors["green"], 255, 0, 0, 0, 0, clock_str, solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
      }
      
      // dire reserve time
      {
        char clock_str[6];
        int mins = std::floor(extraTimeDire / 60);
        int secs = ((int)extraTimeDire) % 60;
        snprintf(clock_str, 6, "%02d:%02d", mins, secs);
        clock_str[5] = '\0';
        
        // draw extra remaining time
        SDL_Rect dstrect;
        dstrect.x = 137;
        dstrect.y = 468;
        dstrect.w = 64;
        dstrect.h = 16;
        SDL_Surface* text_surface = draw_text(font16, screen->format, colors["red"], 255, 0, 0, 0, 0, clock_str, solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
      }
      
      // hero pick state label
      {
// 2, 4, 6, 8, 10
// 0, 2, 4, 6, 8
// 0, 1, 2, 3, 4
      
// B1 7, 9, 11, 13, 16
// B2 8, 10, 12, 14, 15
// P1 17, 20, 22, 24, 26
// P2 18, 19, 21, 23, 25
        int const heroPickStateMap[] = {0,0,0,0,0,0,0,1,1,2,2,3,3,4,4,5,5};
        switch (heroPickState) {
          case 7:
            
          break;
        }
        std::string label = "";
        if (heroPickState >= 7 && heroPickState <= 16) {
          int n = 0;
          if (heroPickState % 2 == 0) {
            n = ((heroPickState - 8) / 2) + 1;
          }
          else {
            n = ((heroPickState - 7) / 2) + 1;
          }
          label = "Ban #" + std::to_string(n);
        }
        else if (heroPickState >= 17 && heroPickState <= 26) {
          int n = 0;
          if (heroPickState % 2 == 0) {
            n = ((heroPickState - 18) / 2) + 1;
          }
          else {
            n = ((heroPickState - 17) / 2) + 1;
          }
          label = "Pick #" + std::to_string(n);
        }
        
        if (label != "") {
          SDL_Rect dstrect;
          dstrect.x = activeTeam == 3 ? 326 : 374;
          dstrect.y = activeTeam == 3 ? 185 : 830;
          dstrect.w = 64;
          dstrect.h = 16;
          SDL_Surface* text_surface = draw_text(font16, 255, 255, 255, 255, 0, 0, 0, 0, label, solid);
          SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
          SDL_FreeSurface(text_surface);
        }
      }
      

    }
    else if (isPrefix(kv.second->className, "CDOTA_DataSpectator")) {
    
    }
    else if (isPrefix(kv.second->className, "CDOTA_DataDire")) {
      for (int i = 0; i < 5; ++i) {
        std::string n = "000" + std::to_string(i);
        int32_t goldUnreliable;
        int32_t goldReliable;
        if (!kv.second->fetchInt32("m_vecDataTeam." + n + ".m_iUnreliableGold", goldUnreliable)) continue;
        if (!kv.second->fetchInt32("m_vecDataTeam." + n + ".m_iReliableGold", goldReliable)) continue;
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_DataRadiant")) {
      for (int i = 0; i < 5; ++i) {
        std::string n = "000" + std::to_string(i);
        int32_t goldUnreliable;
        int32_t goldReliable;
        if (!kv.second->fetchInt32("m_vecDataTeam." + n + ".m_iUnreliableGold", goldUnreliable)) continue;
        if (!kv.second->fetchInt32("m_vecDataTeam." + n + ".m_iReliableGold", goldReliable)) continue;
      }
    }
    else if (isPrefix(kv.second->className, "CDOTA_PlayerResource")) {
      if (gameState < 4) continue;
      
      // header
      {
        std::stringstream header;
        header << "K  |D |A  |LH  |DN |RGOLD|UGOLD|LE|XP  |ST|RS|NET  |XPM |GPM " << std::endl;
            
        SDL_Rect dstrect;
        dstrect.x = 1070;
        dstrect.y = 492;
        dstrect.w = 462;
        dstrect.h = 10;
        SDL_Surface* text_surface = draw_text(font_lucida10, 0, 0, 0, 255, 0, 0, 0, 0, header.str(), solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
      }
      {
        std::stringstream header;
        header << "   |  |   |    |   |     |     |VL|    |RK|PN|WORTH|    |    " << std::endl;
            
        SDL_Rect dstrect;
        dstrect.x = 1070;
        dstrect.y = 502;
        dstrect.w = 462;
        dstrect.h = 10;
        SDL_Surface* text_surface = draw_text(font_lucida10, 0, 0, 0, 255, 0, 0, 0, 0, header.str(), solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
      }
      
      for (int i = 0; i < 10; ++i) {
        std::string n = "000" + std::to_string(i);
        std::string playerName;
        uint64_t steamId;
        uint32_t heroEntId;
        int32_t kills;
        int32_t deaths;
        int32_t assists;
        int32_t streak;
        int32_t level;
        int32_t respawnSeconds;
        
        if (!kv.second->fetchString("m_vecPlayerData." + n + ".m_iszPlayerName", playerName)) continue;
        if (!kv.second->fetchUint64("m_vecPlayerData." + n + ".m_iPlayerSteamID", steamId)) continue;
        if (!kv.second->fetchUint32("m_vecPlayerTeamData." + n + ".m_hSelectedHero", heroEntId)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iKills", kills)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iDeaths", deaths)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iAssists", assists)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iStreak", streak)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iLevel", level)) continue;
        if (!kv.second->fetchInt32("m_vecPlayerTeamData." + n + ".m_iRespawnSeconds", respawnSeconds)) continue;
        
        std::stringstream playerInfo;
        playerInfo << playerName 
          << " KDA: " << std::to_string(kills)
          << "/" << std::to_string(deaths)
          << "/" << std::to_string(assists)
          << " Streak: " << std::to_string(streak)
          << " Level: " << std::to_string(level)
          << " Respawn Time: " << std::to_string(respawnSeconds)
          << std::endl;
        
        SDL_Rect dstrect;
        dstrect.x = 1070;
        dstrect.y = 502 + 16 + 24 * i;
        dstrect.w = 462;
        dstrect.h = 16;
        SDL_Surface* text_surface = draw_text(font_lucida10, 0, 0, 0, 255, 0, 0, 0, 0, playerInfo.str(), solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
        
        heroEntId = heroEntId & 2047;
        if (p.packetEntities.find(heroEntId) == p.packetEntities.end()) continue;

        for (int j = 0; j < 16; ++j) {
          char buf[5];
          sprintf(buf, "%04d", j);
          std::string m = std::string(buf, 4);
          uint32_t abilityEntityId;
          if (!p.packetEntities[heroEntId]->fetchUint32("m_hAbilities." + m, abilityEntityId)) continue;
          abilityEntityId = abilityEntityId & 2047;
          if (abilityEntityId != 2047) {
            if (p.packetEntities.find(abilityEntityId) == p.packetEntities.end()) continue;
            int32_t abilityEntNameIndex;
            p.packetEntities[abilityEntityId]->fetchInt32("CEntityIdentity.m_nameStringableIndex", abilityEntNameIndex);
            StringTable* entityNamesTable = p.stringTables.tables[p.stringTables.nameIndex["EntityNames"]];
            const std::string& abilityEntName = entityNamesTable->items[abilityEntNameIndex]->key;
            if (ability_icons_requested.find(abilityEntName) == ability_icons_requested.end()) {
              ability_icons_requested.insert(abilityEntName);
              emscripten_async_wget(("spellicons24x24/" + abilityEntName + ".png").c_str(), (abilityEntName + ".png").c_str(), on_ability_icon_load, onerror);
              std::cout << abilityEntName << "\n";
            }
            else if (ability_icons.find(abilityEntName) != ability_icons.end()) {
              dstrect.x = 1070 + 24 * j;
              dstrect.y = 248 + 24 * i;
              dstrect.w = 24;
              dstrect.h = 24;
              SDL_BlitSurface(ability_icons[abilityEntName], NULL, screen, &dstrect);
              
              int32_t abilityLevel;
              if (p.packetEntities[abilityEntityId]->fetchInt32("m_iLevel", abilityLevel)) {
                SDL_Surface* text_surface = draw_text(font, 0, 0, 0, 255, 0, 0, 0, 0, std::to_string(abilityLevel), solid);
                SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
                SDL_FreeSurface(text_surface);
              }
              
              float cooldown;
              if (p.packetEntities[abilityEntityId]->fetchFloat32("m_fCooldown", cooldown)) {
                int cooldownSeconds = int(cooldown - gameTime);
                dstrect.y = i * 32 + 16;
                SDL_Surface* text_surface = draw_text(font, 0, 0, 0, 255, 0, 0, 0, 0, std::to_string(cooldownSeconds), solid);
                SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
                SDL_FreeSurface(text_surface);
              }
            }
          }
        }
        
        SDL_Surface* small_portrait_icon = small_portrait_icons.get_image(p.packetEntities[heroEntId]->className);
        if (small_portrait_icon != nullptr) {
          dstrect.x = 1028;
          dstrect.y = 248 + 24 * i;
          dstrect.w = 42;
          dstrect.h = 24;
          SDL_BlitSurface(small_portrait_icon, NULL, screen, &dstrect);

          dstrect.y = 4 + 24 * i;
          SDL_BlitSurface(small_portrait_icon, NULL, screen, &dstrect);
        }

        dstrect.w = 33;
        dstrect.h = 23;
        for (int j = 0; j < 14; ++j) {
          dstrect.x = 1070 + 33 * j;
          std::string k = "000" + std::to_string(j);
          uint32_t itemEntId;
          if (!p.packetEntities[heroEntId]->fetchUint32("m_hItems." + k, itemEntId)) {
            SDL_BlitSurface(itemsprites, &(h_offsets[item_emptyitembg]), screen, &dstrect);
          }
          itemEntId = itemEntId & 2047;
          if (itemEntId == 2047) {
            SDL_BlitSurface(itemsprites, &(h_offsets[item_emptyitembg]), screen, &dstrect);
          }
          else {
            if (p.packetEntities.find(itemEntId) == p.packetEntities.end()) continue;
            int32_t itemEntNameIndex;
            p.packetEntities[itemEntId]->fetchInt32("CEntityIdentity.m_nameStringableIndex", itemEntNameIndex);
            StringTable* entityNamesTable = p.stringTables.tables[p.stringTables.nameIndex["EntityNames"]];
            const std::string& itemEntName = entityNamesTable->items[itemEntNameIndex]->key;
            SDL_BlitSurface(itemsprites, &(h_offsets[item_map[itemEntName]]), screen, &dstrect);
          }
        }
      }
    }
  }

  SDL_Flip(screen);
  replayTick += playbackSpeed;
  
  EM_ASM_({
    updateSlider($0);
  }, p.tick);
}