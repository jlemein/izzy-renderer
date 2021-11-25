//
// Created by jeffrey on 16-11-21.
//
#pragma once

#include <geo_textureloader.h>
#include <filesystem>

namespace lsw {

namespace geo {
struct Texture;
}

namespace georm {

/**
 * @brief Loads textures using the Stb image loader library. This means this texture loader
 * is mainly suited for common texture formats supporting the RGB(A) image format, such
 * as jpg, png, hdr, bmp.
 * @see getSupportedExtensions for a list of supported extensions.
 */
class StbTextureLoader : public geo::TextureLoader {
 public:
  StbTextureLoader(bool flipVertical = false);
  virtual ~StbTextureLoader() = default;

  /// @inherit
  geo::Texture loadTexture(const std::filesystem::path& path) override;

  /// @inherit
  geo::ExtensionList getSupportedExtensions() const override;

 private:
  bool m_flipVertical {false};
};

}  // namespace georm
}  // namespace lsw
