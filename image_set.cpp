#include "image_set.hpp"

ImageSet::ImageSet(std::string _image_dir, std::string _image_ext, em_str_callback_func _onload, em_str_callback_func _onerror) {
  image_dir = _image_dir;
  image_ext = _image_ext;
  onload = _onload;
  onerror = _onerror;
}

SDL_Surface* ImageSet::get_image(std::string key) {
  if (requested.find(key) == requested.end()) {
    requested.insert(key);
    emscripten_async_wget((image_dir + key + image_ext).c_str(), (key + image_ext).c_str(), onload, onerror);
  }
  else if (images.find(key) != images.end()) {
    return images[key];
  }
  return nullptr;
}