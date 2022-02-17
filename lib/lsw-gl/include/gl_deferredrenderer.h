//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>
#include <memory>
#include <gl_renderable.h>

namespace izz {

class SceneGraph;

namespace gl {

class RenderSystem;

struct DeferredRenderable {
  BufferedMeshData meshData;
  int materialId; /// references material in material system.
  int renderStateId {-1};

  int mvpUboIndex;      /// Index in the renderState uniforms corresponding to MVP uniform buffer object.
  int lightingUboIndex; /// Index in the renderState uniforms corresponding to light uniform buffer object.
};

class DeferredRenderer {
 public:
  DeferredRenderer(RenderSystem& renderSystem,
                   std::shared_ptr<izz::SceneGraph> sceneGraph);

  void init();

  void update();

  void render(const entt::registry& registry);


 private:
  int m_fbo{0};
  RenderSystem& m_renderSystem;
  std::shared_ptr<izz::SceneGraph> m_sceneGraph;

  GLuint m_gBufferFbo, m_lightingPassFbo;
  GLuint m_gPosition, m_gNormal, m_gAlbedoSpec;
};

}  // namespace gl
}  // namespace izz
