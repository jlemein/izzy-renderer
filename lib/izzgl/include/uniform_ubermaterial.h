//
// Created by jlemein on 07-03-21.
//

#ifndef RENDERER_UNIFORM_UBERMATERIAL_H
#define RENDERER_UNIFORM_UBERMATERIAL_H

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

  static inline const char* PARAM_NAME = "UberMaterial";
};

class UberUniformManager : public UniformBlockManager {
 public:
  void* CreateUniformBlock(size_t& t) override;
  void DestroyUniformBlock(void* block) override;
  void UpdateUniform(void* data, const gl::Material& material) override;
};

}  // namespace ufm
}  // namespace lsw

#endif  // RENDERER_UNIFORM_UBERMATERIAL_H
