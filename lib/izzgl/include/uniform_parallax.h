#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

struct Parallax {
  float height_scale;

  static inline const char* BUFFER_NAME = "Parallax";
};

class ParallaxManager : public izz::gl::IUniformBuffer {
 public:
  void* allocate(size_t& t) override {
    t = sizeof(Parallax);
    return new Parallax;
  }

  void destroy(void* data) override {
    auto parallax = reinterpret_cast<Parallax*>(data);
    delete parallax;
  }

  virtual void onInit() {}

  void onUpdate(void* data, const gl::Material& material, float dt, float time) override {
    auto parallax = reinterpret_cast<Parallax*>(data);
    parallax->height_scale = material.getUniformFloat("height_scale");
  }

  virtual void onFrameStart(float dt, float time) {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};



}
}  // namespace lsw