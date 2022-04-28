#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
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
  void UpdateUniform(void* data, const izz::gl::Material& m) override {
    auto constant = reinterpret_cast<ConstantLight*>(data);
    constant->radius = m.getUniformFloat("radius");
    constant->intensity = m.getUniformFloat("intensity");
    constant->color = m.getUniformVec4("color");
  }
};



}
}  // namespace lsw