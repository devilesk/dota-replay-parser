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
int minuteTick = -1;
int gameState;
std::unordered_map<std::string, uint32_t> colors;
std::unordered_map<uint64_t, uint32_t> team_colors;

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
  p.skipTo(50322); //14322
  replayTick = 50322;
  //p.handle();
  
  init_sprites();
  load_images();
  
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(1536, 952, 32, SDL_SWSURFACE);
  
  init_color_map();
  
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
                      uint32_t bg, uint8_t bgA, std::string& text, textquality quality) {
                      
    uint8_t fgR, fgG, fgB, bgR, bgG, bgB;
    SDL_GetRGB(fg, format, &fgR, &fgG, &fgB);
    SDL_GetRGB(bg, format, &bgR, &bgG, &bgB);

    return draw_text(fonttodraw, fgR, fgG, fgB, fgA, bgR, bgG, bgB, bgA, text, quality);
}

SDL_Surface* draw_text(TTF_Font *fonttodraw, const SDL_PixelFormat* format, uint32_t fg, uint8_t fgA, 
                      uint8_t bgR, uint8_t bgG, uint8_t bgB, uint8_t bgA, std::string& text, textquality quality) {
                      
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
      SDL_FillRect(screen, &dstrect, team_colors[team]);
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
      float gameTime;         // current time
      float gameStartTime;    // time when clock reached 0
      int32_t gameMode;       // Game Mode
      int32_t _gameState;     // Game State
      int32_t netTimeOfDay;   // Time of Day
      uint32_t heroPickState; // Hero Pick state
      int32_t activeTeam;     // Active Team   
                              // 2: Radiant is currently picking/banning.
                              // 3: Dire is currently picking/banning.
      int32_t startingTeam;   // Starting Team
                              // 0: Not a CM or reverse-CM game.
                              // 2: Radiant has first pick.
                              // 3: Dire has first pick.

      kv.second->fetchFloat32("CDOTAGamerules.m_fGameTime", gameTime);
      kv.second->fetchFloat32("CDOTAGamerules.m_flGameStartTime", gameStartTime);
      kv.second->fetchInt32("CDOTAGamerules.m_iGameMode", gameMode);
      kv.second->fetchInt32("CDOTAGamerules.m_nGameState", _gameState);
      kv.second->fetchInt32("CDOTAGamerules.m_iNetTimeOfDay", netTimeOfDay);
      kv.second->fetchUint32("CDOTAGamerules.m_nHeroPickState", heroPickState);
      kv.second->fetchInt32("CDOTAGamerules.m_iActiveTeam", activeTeam);
      kv.second->fetchInt32("CDOTAGamerules.m_iStartingTeam", startingTeam);

      for (int i = 0; i < 10; ++i) {
        std::string n = "000" + std::to_string(i);
        int32_t bannedHero;
        int32_t pickedHero;
        kv.second->fetchInt32("CDOTAGamerules.m_BannedHeroes.000" + n, bannedHero);
        kv.second->fetchInt32("CDOTAGamerules.m_SelectedHeroes.000" + n, pickedHero);
      }
      
      int clock = 0;
      if (gameStartTime > 1) {
        clock = int(gameTime-gameStartTime);
        int mins = std::floor(clock / 60);
        if (((int)clock) % 60 == 0 && mins > minuteTick) {
          minuteTick = mins;
          /*EM_ASM_({
              addSliderTick($0);
          }, msg->tick);*/
        }
      }
      
      // create clock string
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
      
      if (gameState != _gameState) {
        gameState = (int)_gameState;
        std::cout << "stateChange" << ", " << std::to_string(gameState) << ", " << std::to_string(clock) << ", " << std::to_string(p.tick) << std::endl;
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
        dstrect.x = 1024;
        dstrect.y = 32 * i;
        dstrect.w = 200;
        dstrect.h = 32;
        SDL_Surface* text_surface = draw_text(font, 0, 0, 0, 255, 0, 0, 0, 0, playerInfo.str(), solid);
        SDL_BlitSurface(text_surface, NULL, screen, &dstrect);
        SDL_FreeSurface(text_surface);
        
        heroEntId = heroEntId & 2047;
        if (p.packetEntities.find(heroEntId) == p.packetEntities.end()) continue;

        dstrect.x = 1030;
        dstrect.y = 33 + 34 * 11 + 36 * i;
        dstrect.w = 32;
        dstrect.h = 32;
        SDL_BlitSurface(herosprites, &(h_offsets[hero_map[p.packetEntities[heroEntId]->className]]), screen, &dstrect);
        
        dstrect.h = 36;
        dstrect.w = 47;
        dstrect.y = 33 + 34 * 11 + 36 * i;
        for (int j = 0; j < 6; ++j) {
          dstrect.x = 1030 + 34 + 47 * j;
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
  ++replayTick;
}