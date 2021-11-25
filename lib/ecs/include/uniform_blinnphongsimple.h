#pragma once

#include <geo_material.h>
#include <uniform_uniformblockmanager.h>
namespace lsw {
namespace ufm {

struct BlinnPhongSimple {
  glm::vec4 albedo;
  glm::vec4 specular;
  glm::vec4 roughness;
  float max_shininess;

  static inline const char* PARAM_NAME = "BlinnPhongSimple";
};

class BlinnPhongSimpleManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(ConstantLight);
    return new ConstantLight;
  }

  void DestroyUniformBlock(void* data) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    delete blinn;
  }
  void UpdateUniform(void* data, const geo::Material& m) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    blinn->max_shininess = m.userProperties.getFloat("max_shininess");
    blinn->albedo = m.userProperties.getVec4f("albedo");
    blinn->specular = m.userProperties.getVec4f("specular");
    blinn->roughness = m.userProperties.getVec4f("roughness");
  }
};



}
}  // namespace lsw