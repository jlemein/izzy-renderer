#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct DeferredLighting {
  static constexpr int MAX_POINT_LIGHTS = 32;

  glm::vec3 viewPos;
  int numberOfLights {0};

  struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float radius;

    float _padding[2];
  } pointLights[32];

  static inline const char* PARAM_NAME = "DeferredLighting";
};

class DeferredLightingManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(DeferredLighting);
    return new DeferredLighting;
  }

  void DestroyUniformBlock(void* data) override {
    auto deferredlighting = reinterpret_cast<DeferredLighting*>(data);
    delete deferredlighting;
  }
  void UpdateUniform(void* data, const gl::Material& m) override {
    // do nothing
  }
};



}
}  // namespace lsw