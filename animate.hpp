#ifndef _ANIMATE_HPP_
#define _ANIMATE_HPP_

#include <unordered_map>
#include <sstream>
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "parser.hpp"
#include "spriteclips.hpp"

enum textquality {solid, shaded, blended};
void init_color_map();
TTF_Font* load_font(const char* file, int ptsize);
SDL_Surface* draw_text(TTF_Font *fonttodraw, uint8_t fgR, uint8_t fgG, uint8_t fgB, uint8_t fgA, 
                      uint8_t bgR, uint8_t bgG, uint8_t bgB, uint8_t bgA, std::string text, textquality quality);
SDL_Surface* draw_text(TTF_Font *fonttodraw, const SDL_PixelFormat* format, uint32_t fg, uint8_t fgA, 
                      uint32_t bg, uint8_t bgA, std::string& text, textquality quality);
SDL_Surface* draw_text(TTF_Font *fonttodraw, const SDL_PixelFormat* format, uint32_t fg, uint8_t fgA, 
                      uint8_t bgR, uint8_t bgG, uint8_t bgB, uint8_t bgA, std::string& text, textquality quality);
SDL_Surface* load_image(std::string path);
void load_images();
bool has_coordinates(PacketEntity* pe);
bool getCoordinates(PacketEntity* pe, int& img_x, int& img_y);
inline bool getMaxHealth(PacketEntity* pe, uint64_t& maxHealth) {
  return !pe->fetchUint64("m_iMaxHealth", maxHealth);
}
inline bool getTeam(PacketEntity* pe, uint64_t& team) {
  return !pe->fetchUint64("m_iTeamNum", team);
}
void main_loop();

#endif /* _ANIMATE_HPP_ */