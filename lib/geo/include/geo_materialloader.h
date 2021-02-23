//
// Created by jlemein on 10-02-21.
//

#ifndef RENDERER_GEO_MATERIALLOADER_H
#define RENDERER_GEO_MATERIALLOADER_H

#include <res_resourcefactory.h>
#include <geo_mesh.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace affx {
namespace geo {


class MaterialSystem : public affx::res::ResourceFactory {
public:
  MaterialSystem(const std::string& path);

  void initialize();

  std::unique_ptr<res::IResource> loadResource(const std::string &name) override;

  //void setDefaultMaterial(std::shared_ptr<Material> material);
  void setDefaultMaterial(const std::string& name);

  std::unique_ptr<res::IResource> makeDefaultMaterial();

private:
  std::string m_materialConfigUri;
  std::unordered_map<std::string, geo::Material> m_materials;

  std::string m_defaultMaterial {""};
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_MATERIALLOADER_H
