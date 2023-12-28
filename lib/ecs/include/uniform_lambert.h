#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <uniform_uniformblockmanager.h>

namespace lsw {
namespace ufm {

struct Lambert {
  static inline const char* PARAM_NAME = "Lambert";
};

class LambertUniformManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override {
    t = sizeof(Lambert);
    return new Lambert;
  }

  void DestroyUniformBlock(void* block) override {
    auto lambert = reinterpret_cast<Lambert*>(block);
    delete lambert;
  }

  void UpdateUniform(void* block, const geo::Material& material) override {}


};

}  // namespace ufm
}  // namespace lsw
