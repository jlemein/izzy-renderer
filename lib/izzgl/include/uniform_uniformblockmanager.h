#pragma once

namespace izz {
namespace gl {
struct Material;
}
namespace ufm {

/**
 * The parameters which are defined in the material need to be layed out in a structure that is passed to the shader.
 * This class manages those implementations.
 */
class UniformBlockManager {
 public:
  virtual ~UniformBlockManager() = default;

  virtual void* CreateUniformBlock(std::size_t& t) = 0;
  virtual void DestroyUniformBlock(void* block) = 0;
  virtual void UpdateUniform(void* data, const gl::Material& material) = 0;

  // in order to the let the uniform work properly in the UI
  // and to provide semantic validation of your input arguments
//  virtual ValueSemantic GetSemantic(const char* name, float* min, float* max) {
//    return ValueSemantic::UNDEFINED;
//  }
};

}  // namespace ufm
}  // namespace lsw