#pragma once

#include "izzgl_material.h"
#include "uniform_uniformblockmanager.h"
namespace izz {
namespace ufm {

struct Parallax {
  float height_scale;

  static inline const char* PARAM_NAME = "Parallax";
};

class ParallaxManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(Parallax);
    return new Parallax;
  }

  void DestroyUniformBlock(void* data) override {
    auto parallax = reinterpret_cast<Parallax*>(data);
    delete parallax;
  }
  void UpdateUniform(void* data, const gl::Material& m) override {
    auto parallax = reinterpret_cast<Parallax*>(data);
    parallax->height_scale = m.userProperties.getFloat("height_scale");
  }

//  ValueSemantic GetSemantic(const char* name) override {
//    return ValueSemantic::COLOR_RGB
//  }
};



}
}  // namespace lsw