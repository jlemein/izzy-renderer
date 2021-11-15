//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_TEXTURELOADER_H
#define RENDERER_GEO_TEXTURELOADER_H

#include <res_resourcefactory.h>
#include <res_resource.h>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace lsw {
namespace geo {

struct Texture;

using ExtensionList = std::vector<std::string>;

class ITextureLoader {
public:
    virtual geo::Texture loadTexture(const std::filesystem::path& path) = 0;

    virtual ExtensionList getSupportedExtensions() const = 0;
};

class TextureLoader : public lsw::res::ResourceFactory {
public:
  std::unique_ptr<lsw::res::IResource> createResource(const std::string &name) override;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_TEXTURELOADER_H
