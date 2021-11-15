//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include <geo_textureloader.h>
#include <geo_texture.h>

namespace lsw {
namespace georm {

/**
 * Loads EXR high dynamic range images.
 */
class ExrLoader : public geo::ITextureLoader {
public:
    geo::Texture loadTexture(const std::filesystem::path &path) override;

    geo::ExtensionList getSupportedExtensions() const override;
};

} // end of package
} // end of enterprise
