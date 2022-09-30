#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct ConstantLight {
  glm::vec4 color;
  float intensity;
  float radius;

  static inline const char* BUFFER_NAME = "ConstantLight";
};

class ConstantManager : public gl::IUniformBuffer {
 public:
  void* allocate(size_t& t) override {
    t = sizeof(ConstantLight);
    return new ConstantLight;
  }

  void destroy(void* data) override {
    auto constant = reinterpret_cast<ConstantLight*>(data);
    delete constant;
  }

  virtual void onInit() {}

  void onUpdate(void* data, const izz::gl::Material& m, float dt, float time) override {
    auto constant = reinterpret_cast<ConstantLight*>(data);
    constant->radius = m.getUniformFloat("ConstantLight.radius");
    constant->intensity = m.getUniformFloat("ConstantLight.intensity");
    constant->color = m.getUniformVec4("ConstantLight.color");
  }

  virtual void onFrameStart(float dt, float time) {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};



}
}  // namespace lsw