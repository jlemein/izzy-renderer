//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>

namespace lsw {
namespace glrs {
class RenderSystem;
}
}  // namespace lsw

namespace izz {
namespace gl {

struct ForwardRenderable {};

class ForwardRenderer {
 private:
  int m_fbo{0};
  lsw::glrs::RenderSystem& m_renderSystem;

 public:
  ForwardRenderer(glrs::RenderSystem& renderSystem);
  void render(const entt::registry& registry);
};

}  // namespace gl
}  // namespace izz
