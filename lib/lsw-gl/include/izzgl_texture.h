//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <GL/glew.h>
#include <izz.h>
#include <cstdint>
#include <string>
#include <vector>


namespace izz {
namespace gl {

struct Rgba8888 {
  uint8_t r, g, b, a;
};

struct Texture {
  /// @brief Unique id to fetch texture from texture system
  TextureId id{-1};

  /// @brief Texture buffer id, which is retrieved using a call to glGenTextures()
  GLuint bufferId {0U};

  std::string path;
  int width{0};
  int height{0};
  int depth{1};
  int channels{0};
  std::vector<uint8_t> data;
};

}  // namespace gl
}  // namespace izz
