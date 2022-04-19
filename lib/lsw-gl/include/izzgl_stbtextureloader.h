//
// Created by jeffrey on 16-11-21.
//
#pragma once

#include <filesystem>
#include "izzgl_textureloader.h"

namespace izz {
namespace gl {
struct Texture;

/**
 * @brief Loads textures using the Stb image loader library. This means this texture loader
 * is mainly suited for common texture formats supporting the RGB(A) image format, such
 * as jpg, png, hdr, bmp.
 * @see getSupportedExtensions for a list of supported extensions.
 */
class StbTextureLoader : public TextureLoader {
 public:
  StbTextureLoader(bool flipVertical = false);
  virtual ~StbTextureLoader() = default;

  /// @inherit
  Texture loadTexture(const std::filesystem::path& path) override;

  /// @inherit
  ExtensionList getSupportedExtensions() const override;

 private:
  bool m_flipVertical{false};
};

}  // namespace gl
}  // namespace izz
