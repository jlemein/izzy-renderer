#pragma once

#include <ecs_camera.h>
#include <ecs_transform.h>
#include <izzgl_material.h>
#include <izzgl_scenedependentuniform.h>
#include "izz_skybox.h"

namespace izz {
namespace ufm {

/**
 * CPU based uniform data that maps the uniform data with the GLSL shader.
 */
struct GammaCorrection {
  /// inverted gamma. Usually gamma is 2.2.
  float invGamma = 1.0/2.2;

  /// name of the uniform buffer in the shader object.
  static inline const char* BUFFER_NAME = "GammaCorrection";
};

class GammaCorrectionUniformUpdater : public izz::gl::IUniformBuffer {
 public:
  GammaCorrectionUniformUpdater() = default;

  virtual ~GammaCorrectionUniformUpdater() = default;

  virtual izz::gl::UniformBufferPtr allocate(size_t& t) override {
    t = sizeof(GammaCorrection);
    return new GammaCorrection;
  }

  virtual void destroy(izz::gl::UniformBufferPtr data) override {
    auto gammaCorrection = reinterpret_cast<GammaCorrection*>(data);
    delete gammaCorrection;
  }

  virtual void onUpdate(void* data, const gl::Material& m, float dt, float time) override {}

  virtual void onFrameStart(float dt, float time) override {}

  virtual void onEntityUpdate(entt::entity e, izz::gl::Material& material) override {}
};

}  // namespace ufm
}  // namespace izz