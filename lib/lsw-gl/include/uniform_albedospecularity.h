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

  static inline const char* PARAM_NAME = "AlbedoSpecularity";
};

class AlbedoSpecularityManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(AlbedoSpecularity);
    return new AlbedoSpecularity;
  }

  void DestroyUniformBlock(void* data) override {
    auto constant = reinterpret_cast<AlbedoSpecularity*>(data);
    delete constant;
  }
  void UpdateUniform(void* data, const izz::gl::Material& m) override {
    auto uniform = reinterpret_cast<AlbedoSpecularity*>(data);
    glm::vec4 albedo = m.getUniformVec4("albedo");
    float specularity = m.getUniformFloat("specularity");
    uniform->albedo_specularity = glm::vec4(albedo.x, albedo.y, albedo.z, specularity);
  }
};

}
}  // namespace lsw