#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct BlinnPhongSimple {
  glm::vec4 albedo;
  glm::vec4 specular;
  float shininess;

  static inline const char* PARAM_NAME = "BlinnPhongSimple";
};

class BlinnPhongSimpleManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(BlinnPhongSimple);
    return new BlinnPhongSimple;
  }

  void DestroyUniformBlock(void* data) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    delete blinn;
  }
  void UpdateUniform(void* data, const gl::Material& m) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    blinn->shininess = m.getUniformFloat("shininess");
    blinn->albedo = m.getUniformVec4("albedo");
    blinn->specular = m.getUniformVec4("specular");
  }
};



}
}  // namespace lsw