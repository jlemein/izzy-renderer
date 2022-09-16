//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <gl_renderable.h>
#include <entt/entt.hpp>
#include <memory>
#include "gl_renderutils.h"

namespace izz {
namespace gl {

class RenderSystem;

struct DeferredRenderable {
  //  BufferedMeshData meshData;
  MaterialId materialId{-1};
  MeshBufferId meshBufferId {-1};

  //  UniformBufferMapping mvp;
  //  UniformBufferMapping lights;
//  entt::entity meshEntity{entt::null};
};

class DeferredRenderer {
  static constexpr inline const char* ID = "DeferredRenderer";

 public:
  DeferredRenderer(RenderSystem& renderSystem, entt::registry& registry);

  /// @brief Called when an gl::DeferredRenderable component is added to an entity.
  /// Used to initialize the underlying data structures.
  void onConstruct(entt::registry& r, entt::entity e);

  void init(int width, int height);

  /**
   * Creates a G-Buffer with dimensions width x height. The dimensions imply the texture dimensions
   * of the individual components of a G-buffer: position, normal, color and specularity.
   *
   * @param width The width of the GBuffer.
   * @param height The height of the GBuffer.
   */
  void createGBuffer(int width, int height);

  /**
   * Creates a screen space rectangle, used to render onto when rendering the lighting pass.
   * The lighting pass is the second, or last pass of the deferred rendering method.
   */
  void createScreenSpaceRect();

  void update();

  void render(const entt::registry& registry);

  /**
   * Should be called whenever the framebuffer needs resizing. For example after a window resize event.
   * @param width Width of the new window size.
   * @param height Height of the new window size.
   */
  void resize(int width, int height);

  inline void setActiveCamera(entt::entity cameraEntity) {
    m_activeCamera = cameraEntity;
  }

 private:
  void updateUniformLightingParams();

  int m_fbo{0};
  RenderSystem& m_renderSystem;
  entt::registry& m_registry;

  GLuint m_gBufferFbo, m_lightingPassFbo;

  /// Texture id's (obtained via glGenTextures)
  GLuint m_gPosition, m_gNormal, m_gTangent, m_gAlbedoSpec;
  GLuint m_depthBuffer;

  /// Uniform locations in shader (obtained via glGetUniformLocation).
  GLint m_gPositionLoc = -1, m_gNormalLoc = -1, m_gTangentLoc = -1, m_gAlbedoSpecLoc = -1;
  entt::entity m_activeCamera = entt::null;

  int m_screenWidth = 10, m_screenHeight = 10;

  MeshBufferId m_screenSpaceMeshBufferId {-1};
  MaterialId m_lightPassMaterialId{-1};
};

}  // namespace gl
}  // namespace izz
