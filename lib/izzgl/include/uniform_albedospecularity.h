//
// Created by jeffrey on 3-5-22.
//
#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct AlbedoSpecularity {
  glm::vec4 albedo_specularity;

  static inline const char* BUFFER_NAME = "AlbedoSpecularity";
};

class AlbedoSpecularityManager : public gl::IUniformBuffer {
 public:
  virtual void* allocate(size_t& t) override {
    t = sizeof(AlbedoSpecularity);
    return new AlbedoSpecularity;
  }

  virtual void destroy(void* data) override {
    auto constant = reinterpret_cast<AlbedoSpecularity*>(data);
    delete constant;
  }

  virtual void onInit() {}

  virtual void onUpdate(void* data, const izz::gl::Material& material, float dt, float time) override {
    auto uniform = reinterpret_cast<AlbedoSpecularity*>(data);
    glm::vec4 albedo = material.getUniformVec4("albedo");
    float specularity = material.getUniformFloat("specularity");
    uniform->albedo_specularity = glm::vec4(albedo.x, albedo.y, albedo.z, specularity);
  }

  virtual void onFrameStart(float dt, float time){};
  virtual void onEntityUpdate(entt::entity e, gl::Material& material) {}
};

}  // namespace ufm
}  // namespace izz