#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

struct DebugProperties {
  glm::vec4 color{1.0F, 1.0F, 1.0F, 1.0F};

  static inline const char* BUFFER_NAME = "DebugProperties";
};

class DebugPropertiesUniform : public gl::IUniformBuffer {
 public:
  virtual void* allocate(size_t& t) override {
    t = sizeof(DebugProperties);
    return new DebugProperties;
  }

  virtual void destroy(void* data) override {
    auto prop = reinterpret_cast<DebugProperties*>(data);
    delete prop;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* data, const gl::Material& material, float dt, float time) override {
    auto prop = reinterpret_cast<DebugProperties*>(data);
    prop->color = material.getUniformVec4("color");
  }
  virtual void onFrameStart(float dt, float time){};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};

}  // namespace ufm
}  // namespace izz