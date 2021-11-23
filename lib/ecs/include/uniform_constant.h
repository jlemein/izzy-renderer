#pragma once

#include <geo_material.h>
#include <uniform_uniformblockmanager.h>
namespace lsw {
namespace ufm {

struct Constant {
  int lightIndex;

  static inline const char* PARAM_NAME = "ConstantLight";
};

class ConstantManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(Constant);
    return new Constant;
  }

  void DestroyUniformBlock(void* data) override {
    auto constant = reinterpret_cast<Constant*>(data);
    delete constant;
  }
  void UpdateUniform(void* data, const geo::Material& m) override {
    auto constant = reinterpret_cast<Constant*>(data);
    constant->lightIndex = m.userProperties.getInt("light_index");
  }

//  ValueSemantic GetSemantic(const char* name) override {
//    return ValueSemantic::COLOR_RGB
//  }
};



}
}  // namespace lsw