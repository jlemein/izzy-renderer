//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/entt.hpp>
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

  entt::entity meshEntity {entt::null};
};

class DeferredRenderer {
 public:
  DeferredRenderer(RenderSystem& renderSystem,
                   entt::registry& registry);

  /// @brief Called when an gl::DeferredRenderable component is added to an entity.
  /// Used to initialize the underlying data structures.
  void onConstruct(entt::registry& r, entt::entity e);

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
