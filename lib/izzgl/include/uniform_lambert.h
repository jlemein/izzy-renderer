#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

struct Lambert {
  static inline const char* BUFFER_NAME = "Lambert";
};

//class LambertUniformManager : public UniformBlockManager {
class LambertUniformManager : public izz::gl::IUniformBuffer {
 public:
  void* allocate(size_t& t) override {
    t = sizeof(Lambert);
    return new Lambert;
  }

  void destroy(void* block) override {
    auto lambert = reinterpret_cast<Lambert*>(block);
    delete lambert;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* block, const gl::Material& material, float dt, float time) override {}
  virtual void onFrameStart(float dt, float time) {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}

};

}  // namespace ufm
}  // namespace lsw
