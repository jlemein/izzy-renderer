#pragma once

#include <geo_material.h>
#include <uniform_uniformblockmanager.h>
namespace lsw {
namespace ufm {

struct ConstantLight {
  glm::vec4 color;
  float intensity;
  float radius;

  static inline const char* PARAM_NAME = "ConstantLight";
};

class ConstantManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(ConstantLight);
    return new ConstantLight;
  }

  void DestroyUniformBlock(void* data) override {
    auto constant = reinterpret_cast<ConstantLight*>(data);
    delete constant;
  }
  void UpdateUniform(void* data, const geo::Material& m) override {
    auto constant = reinterpret_cast<ConstantLight*>(data);
    constant->radius = m.userProperties.getFloat("radius");
    constant->intensity = m.userProperties.getFloat("intensity");
    constant->color = m.userProperties.getVec4f("color");
  }
};



}
}  // namespace lsw