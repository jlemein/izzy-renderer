#pragma once

#include "izzgl_material.h"
#include <izzgl_scenedependentuniform.h>

namespace izz {
namespace ufm {

struct BlinnPhongSimple {
  glm::vec4 albedo;
  glm::vec4 specular;
  float shininess;

  static inline const char* BUFFER_NAME = "BlinnPhongSimple";
};

class BlinnPhongSimpleManager : public gl::IUniformBuffer {
 public:
  virtual void* allocate(size_t& t) override {
    t = sizeof(BlinnPhongSimple);
    return new BlinnPhongSimple;
  }

  virtual void destroy(void* data) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    delete blinn;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* data, const gl::Material& material, float dt, float time) override {
    auto blinn = reinterpret_cast<BlinnPhongSimple*>(data);
    blinn->shininess = material.getUniformFloat("shininess");
    blinn->albedo = material.getUniformVec4("albedo");
    blinn->specular = material.getUniformVec4("specular");
  }

  virtual void onFrameStart(float dt, float time) {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};



}
}  // namespace lsw