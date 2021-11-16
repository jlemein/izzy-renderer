//
// Created by jeffrey on 15-11-21.
//
#pragma once

#include <geo_textureloader.h>

namespace lsw {

namespace geo {
struct Texture;
}

namespace georm {

/**
 * Loads EXR high dynamic range images.
 */
class ExrLoader : public geo::TextureLoader {
public:
    virtual ~ExrLoader() = default;
    geo::Texture loadTexture(const std::filesystem::path &path) override;

    geo::ExtensionList getSupportedExtensions() const override;
};

} // end of package
} // end of enterprise
