//
// Created by jeffrey on 22-12-22.
//
#include <izz_texture.h>
#include <ostream>
using namespace izz::gl;

std::ostream& operator<<(std::ostream& out, const TextureType value) {
  static std::map<TextureType, std::string> strings;

  if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
    INSERT_ELEMENT(TextureType::UNDEFINED);
    INSERT_ELEMENT(TextureType::TEXTURE_2D);
    INSERT_ELEMENT(TextureType::CUBEMAP);
#undef INSERT_ELEMENT
  }

  return out << strings[value];
}