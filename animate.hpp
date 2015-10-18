#ifndef _ANIMATE_HPP_
#define _ANIMATE_HPP_

#include "spriteclips.hpp"

SDL_Surface* load_image();
void load_images();
bool has_coordinates(PacketEntity* pe);
void get_coordinates(PacketEntity* pe, int& img_x, int& img_y);
void main_loop();

#endif /* _ANIMATE_HPP_ */