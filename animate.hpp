#ifndef _ANIMATE_HPP_
#define _ANIMATE_HPP_

#include "spriteclips.hpp"

SDL_Surface* load_image();
void load_images();
bool has_coordinates(PacketEntity* pe);
bool getCoordinates(PacketEntity* pe, int& img_x, int& img_y);
bool getMaxHealth(PacketEntity* pe, uint64_t& maxHealth);
void main_loop();

#endif /* _ANIMATE_HPP_ */