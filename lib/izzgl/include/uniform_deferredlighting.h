#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct DeferredLighting {
  static constexpr int MAX_POINT_LIGHTS = 32;
  static constexpr int MAX_DIRECTIONAL_LIGHTS = 2;

  glm::vec3 viewPos;
  int numberOfDirectionalLights {0};
  int numberOfPointLights {0};
  int numberOfSpotLights {0};
  int _padding[2];

  struct DirectionalLight {
    glm::vec4 position;
    glm::vec4 color;
  } directionalLights[MAX_DIRECTIONAL_LIGHTS];

//  struct SpotLight {
//    glm::vec4 position;
//    glm::vec4 color;
//    float penumbra;
//    float umbra;
//  } spotLights[MAX_SPOT_LIGHTS];

  struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float radius;

    float _padding[2];
  } pointLights[MAX_POINT_LIGHTS];

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
    // intentionally left empty. There is no material data that needs to be passed in this structure.
  }
};



}
}  // namespace lsw