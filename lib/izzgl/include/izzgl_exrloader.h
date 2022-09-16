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
  Texture loadTexture(const std::filesystem::path& path) override;

  ExtensionList getSupportedExtensions() const override;

 private:
  bool m_flipVertical{false};
};

}  // namespace gl
}  // namespace izz