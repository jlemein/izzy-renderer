#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

struct BlinnPhong {
  glm::vec4 specular;
  float shininess;

  static inline const char* BUFFER_NAME = "BlinnPhong";
};

class BlinnPhongManager : public gl::IUniformBuffer {
 public:
  virtual void* allocate(size_t& t) override {
    t = sizeof(BlinnPhong);
    return new BlinnPhong;
  }

  virtual void destroy(void* data) override {
    auto blinn = reinterpret_cast<BlinnPhong*>(data);
    delete blinn;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* data, const gl::Material& material, float dt, float time) override {
    auto blinn = reinterpret_cast<BlinnPhong*>(data);
    blinn->shininess = material.getUniformFloat("shininess");
    blinn->specular = material.getUniformVec4("specular_color");
  }
  virtual void onFrameStart(float dt, float time) {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};



}
}  // namespace lsw