//
// Created by jlemein on 07-03-21.
//

#pragma once

#include <izzgl_scenedependentuniform.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

// TODO: this is implementation specific for OpenGL, move it to separate namespace
struct Uber {
  glm::vec4 diffuse{1.0F, 0.0F, 0.0F, 0.0F};
  glm::vec4 specular;
  glm::vec4 ambient;
  bool hasDiffuseTex{false};

  static inline const char* BUFFER_NAME = "UberMaterial";
};

class UberUniformManager : public izz::gl::IUniformBuffer {
 public:
  void* allocate(size_t& t) override;
  void destroy(void* block) override;

  virtual void onInit() override {}
  virtual void onUpdate(void* data, const gl::Material& material, float dt, float time) override;
  virtual void onFrameStart(float dt, float time) override {};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) override {}

};

}  // namespace ufm
}  // namespace izz
