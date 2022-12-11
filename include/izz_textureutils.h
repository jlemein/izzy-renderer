//
// Created by jeffrey on 11-12-22.
//

#pragma once

#include "izz_texture.h"

namespace izz {
namespace gl {

struct TextureUtils {
  static inline int GetChannelCount(TextureFormat format) {
    switch(format) {
      case TextureFormat::R8:
      case TextureFormat::R16F:
      case TextureFormat::R32F:
        return 1;

      case TextureFormat::RGB8:
      case TextureFormat::RGB16F:
      case TextureFormat::RGB32F:
        return 3;

      case TextureFormat::RGBA8:
      case TextureFormat::RGBA16F:
      case TextureFormat::RGBA32F:
        return 4;
    }

    throw std::runtime_error("Cannot find number of channels for given texture format");
  }
};

}  // namespace gl
}  // namespace izz
