//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include "geo_textureloader.h"

namespace lsw {

namespace geo {
struct Texture;
}

/**
 * Loads EXR high dynamic range images.
 */
class ExrLoader : public geo::TextureLoader {
 public:
  ExrLoader(bool flipVertical = false);

  virtual ~ExrLoader() = default;
  geo::Texture loadTexture(const std::filesystem::path& path) override;

  geo::ExtensionList getSupportedExtensions() const override;

 private:
  bool m_flipVertical{false};
};

}  // namespace lsw
