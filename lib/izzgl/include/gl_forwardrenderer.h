//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>

namespace izz {
namespace gl {

class RenderSystem;
struct ForwardRenderable {};

class ForwardRenderer {
 private:
  int m_fbo{0};
  RenderSystem& m_renderSystem;
  entt::entity m_activeCamera;

 public:
  ForwardRenderer(RenderSystem& renderSystem);
  void render(const entt::registry& registry);

  inline void setActiveCamera(entt::entity cameraEntity) {
    m_activeCamera = cameraEntity;
  }
};

}  // namespace gl
}  // namespace izz
