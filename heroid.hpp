#ifndef _HEROID_HPP_
#define _HEROID_HPP_

#include <unordered_map>
#include "spriteclips.hpp"

extern std::unordered_map<int32_t, sprite_t> heroindex_map;
extern std::unordered_map<int32_t, std::string> heroid_map;
std::unordered_map<int32_t, sprite_t> init_heroindex_map();
std::unordered_map<int32_t, std::string> init_heroid_map();

#endif /* _HEROID_HPP_ */