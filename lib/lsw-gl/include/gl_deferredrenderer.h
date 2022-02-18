//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>
#include <memory>
#include <gl_renderable.h>
#include "gl_renderutils.h"

namespace izz {
namespace gl {

class RenderSystem;

struct DeferredRenderable {
  BufferedMeshData meshData;
//  int materialId; /// references material in material system.
  int renderStateId {-1};

  UniformBufferMapping mvp;
  UniformBufferMapping lights;
};

class DeferredRenderer {
 public:
  DeferredRenderer(RenderSystem& renderSystem,
                   entt::registry& registry);

  void init();

  void update();

  void render(const entt::registry& registry);


 private:
  int m_fbo{0};
  RenderSystem& m_renderSystem;
  entt::registry& m_registry;

  GLuint m_gBufferFbo, m_lightingPassFbo;
  GLuint m_gPosition, m_gNormal, m_gAlbedoSpec;
};

}  // namespace gl
}  // namespace izz
