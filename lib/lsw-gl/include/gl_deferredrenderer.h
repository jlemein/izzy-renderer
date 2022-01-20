//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>

namespace lsw {
namespace glrs {
class RenderSystem;
}  // namespace glrs
}  // namespace lsw

namespace izz {
namespace gl {

class DeferredRenderer {
 public:
  DeferredRenderer(const lsw::glrs::RenderSystem& renderSystem);
  void render(const entt::registry& registry);

 private:
  int m_fbo{0};
  const lsw::glrs::RenderSystem& m_renderSystem;
};

}  // namespace gl
}  // namespace izz
