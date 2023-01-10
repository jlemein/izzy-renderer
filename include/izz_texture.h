//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <half.h>
#include <izz.h>
#include <cstdint>
#include <string>
#include <vector>

namespace izz {
namespace gl {

struct Rgba8888 {
  uint8_t r, g, b, a;
};

struct RgbaHdr {
  float r, g, b, a;
};

enum class TextureType { UNDEFINED = 0, TEXTURE_2D, CUBEMAP };

enum class TextureFormat { R8, R16F, R32F, RGB8, RGB16F, RGB32F, RGBA8, RGBA16F, RGBA32F };

enum class TextureMemoryLayout { RGBA8, RGBA16F, RGBA32F };

std::ostream& operator<<(std::ostream& out, const TextureType value);

struct Texture {
  /// @brief Unique id to fetch texture from texture system
  TextureId id{-1};
  TextureType type{TextureType::UNDEFINED};
  TextureFormat format{TextureFormat::RGBA8};

  /// @brief Texture buffer id, which is retrieved using a call to glGenTextures()
  uint32_t bufferId{0U};  // TODO make it a void*, so that each texture system is able to return it's own buffer handle.

  std::string path;      /// Original file path, if applicable. Use name if no path is applicable.
  std::string name{""};  /// Exposed name in UI. Takes precedence over @see path in UI.
  int width{0};
  int height{0};
  int depth{1};
  int numChannels{4};
  std::vector<uint8_t> data;
};

}  // namespace gl
}  // namespace izz
