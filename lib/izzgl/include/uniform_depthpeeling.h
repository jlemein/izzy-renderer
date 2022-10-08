#pragma once

#include <izzgl_scenedependentuniform.h>
#include "izzgl_material.h"

namespace izz {
namespace ufm {

struct DepthPeeling {
  int isPeelingPass;
  int padding_;
  glm::ivec2 screenSize;

  static inline const char* BUFFER_NAME = "DepthPeeling";
};

class DepthPeelingUniformBuffer : public gl::IUniformBuffer {
 public:
  virtual void* allocate(size_t& t) override {
    t = sizeof(DepthPeeling);
    return new DepthPeeling;
  }

  virtual void destroy(void* data) override {
    auto depthPeeling = reinterpret_cast<DepthPeeling*>(data);
    delete depthPeeling;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* data, const gl::Material& material, float dt, float time) override {}

  virtual void onFrameStart(float dt, float time){};

  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};

}  // namespace ufm
}  // namespace izz