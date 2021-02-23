//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_MATERIAL_H
#define RENDERER_GEO_MATERIAL_H

#include <geo_texture.h>
#include <res_resource.h>
#include <memory>

namespace affx {
namespace geo {

struct Material {
  std::string name;

  // colors
  glm::vec3 diffuse;
  glm::vec3 emissive;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 transparent;

  std::shared_ptr<res::Resource<geo::Texture>> diffuseTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> specularTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> normalTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> roughnessTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> opacityTexture {nullptr};

  // geo::ShadingMode shadingMode

  // material
  float shininess;
  float opacity;

};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_MATERIAL_H
