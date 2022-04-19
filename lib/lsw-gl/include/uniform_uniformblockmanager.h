#pragma once

namespace izz {
namespace gl {
struct Material;
}
namespace ufm {

//enum class ValueSemantic {
//  UNDEFINED = 0,
//  POSITION,         /// @brief Value is a position
//  VECTOR_NORMALIZED,  /// @brief The vector is normalized, meaning length must be 1. Good for direction vectors.
//  COLOR_RGB,
//  FLOAT,
//  UNSIGNED_FLOAT,
//  RANGE,
//
//};

class UniformBlockManager {
 public:
  virtual ~UniformBlockManager() = default;

  virtual void* CreateUniformBlock(std::size_t& t) = 0;
  virtual void DestroyUniformBlock(void* block) = 0;
  virtual void UpdateUniform(void* data, const izz::gl::Material& material) = 0;

  // in order to the let the uniform work properly in the UI
  // and to provide semantic validation of your input arguments
//  virtual ValueSemantic GetSemantic(const char* name, float* min, float* max) {
//    return ValueSemantic::UNDEFINED;
//  }
};

}  // namespace ufm
}  // namespace lsw