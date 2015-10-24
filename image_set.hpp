#ifndef _IMAGE_SET_HPP_
#define _IMAGE_SET_HPP_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <emscripten.h>

class ImageSet {
  public:
    std::string image_dir;
    std::string image_ext;
    std::unordered_map<std::string, SDL_Surface*> images;
    std::unordered_set<std::string> requested;
    em_str_callback_func onload;
    em_str_callback_func onerror;
    SDL_Surface* get_image(std::string key);
    ImageSet(std::string _image_dir, std::string _image_ext, em_str_callback_func _onload, em_str_callback_func _onerror);
};

#endif /* _IMAGE_SET_HPP_ */