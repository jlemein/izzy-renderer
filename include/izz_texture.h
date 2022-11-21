//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <izz.h>
#include <cstdint>
#include <string>
#include <vector>

namespace izz {
namespace gl {

struct Rgba8888 {
  uint8_t r, g, b, a;
};

enum class TextureType { UNDEFINED = 0, TEXTURE_2D, CUBEMAP };

struct Texture {
  /// @brief Unique id to fetch texture from texture system
  TextureId id{-1};
  TextureType type{TextureType::UNDEFINED};

  /// @brief Texture buffer id, which is retrieved using a call to glGenTextures()
  uint32_t bufferId{0U};  // TODO make it a void*, so that each texture system is able to return it's own buffer handle.

  std::string path;      /// Original file path, if applicable. Use name if no path is applicable.
  std::string name{""};  /// Exposed name in UI. Takes precedence over @see path in UI.
  int width{0};
  int height{0};
  int depth{1};
  int channels{0};
  std::vector<uint8_t> data;
};

}  // namespace gl
}  // namespace izz
