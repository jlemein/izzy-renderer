#pragma once

namespace lsw {
namespace geo {
struct Material;
}
namespace ufm {

class UniformBlockManager {
 public:
  virtual ~UniformBlockManager() = default;

  virtual void* CreateUniformBlock(std::size_t& t) = 0;
  virtual void DestroyUniformBlock(void* block) = 0;
  virtual void UpdateUniform(void* data, geo::Material& material) = 0;
};

}  // namespace ufm
}  // namespace lsw