//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_TEXTURELOADER_H
#define RENDERER_GEO_TEXTURELOADER_H

#include <res_resourcefactory.h>
#include <res_resource.h>
#include <string>
#include <memory>

namespace affx {
namespace geo {

class TextureLoader : public affx::res::ResourceFactory {
public:
  std::unique_ptr<affx::res::IResource> loadResource(const std::string &name) override;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_TEXTURELOADER_H
