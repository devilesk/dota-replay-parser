#ifndef _ANIMATE_HPP_
#define _ANIMATE_HPP_

#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "parser.hpp"
#include "spriteclips.hpp"

TTF_Font* load_font(const char* file, int ptsize);
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