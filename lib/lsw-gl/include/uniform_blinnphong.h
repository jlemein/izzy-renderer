#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"

namespace izz {
namespace ufm {

struct BlinnPhong {
  glm::vec4 specular;
  float shininess;

  static inline const char* PARAM_NAME = "BlinnPhong";
};

class BlinnPhongManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(BlinnPhong);
    return new BlinnPhong;
  }

  void DestroyUniformBlock(void* data) override {
    auto blinn = reinterpret_cast<BlinnPhong*>(data);
    delete blinn;
  }
  void UpdateUniform(void* data, const gl::Material& m) override {
    auto blinn = reinterpret_cast<BlinnPhong*>(data);
    blinn->shininess = m.getUniformFloat("shininess");
    blinn->specular = m.getUniformVec4("specular_color");
  }
};



}
}  // namespace lsw