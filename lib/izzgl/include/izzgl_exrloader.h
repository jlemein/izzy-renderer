//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include "izzgl_textureloader.h"

namespace izz {
namespace gl {

struct Texture;

/**
 * Loads EXR high dynamic range images.
 */
class ExrLoader : public TextureLoader {
 public:
  ExrLoader(bool flipVertical = false);

  virtual ~ExrLoader() = default;

  /// @inherit
  Texture loadTexture(const std::filesystem::path& path) override;

  /// @inherit
  Texture loadTextureFromMemory(unsigned char* pData, int size) override;

  ExtensionList getSupportedExtensions() const override;

 private:
  bool m_flipVertical{false};
};

}  // namespace gl
}  // namespace izz
