//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <gl_renderable.h>
#include <entt/entt.hpp>
#include <memory>
#include "gl_renderutils.h"
#include "izz_scenegraphentity.h"

namespace izz {
namespace gl {

class MaterialSystem;
class MeshSystem;

struct DeferredRenderable {
  MaterialId materialId{-1};
  VertexBufferId vertexBufferId{-1};
};

class DeferredRenderer {
  static constexpr inline const char* ID = "DeferredRenderer";

 public:
  DeferredRenderer(std::shared_ptr<MaterialSystem> materialSystem, std::shared_ptr<MeshSystem> meshSystem, entt::registry& registry);

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

  /**
   * Updates the materials with new uniform data. This is mainly to sync the MVP matrix that might have been changes
   * because of camera movement.
   *
   * @param dt      Delta time with previous frame (in seconds).
   * @param time    Total time since application start (in seconds).
   */
  void update(float dt, float time);

  void render(const entt::registry& registry);

  /**
   * Should be called whenever the framebuffer needs resizing. For example after a window resize event.
   * @param width Width of the new window size.
   * @param height Height of the new window size.
   */
  void resize(int width, int height);

  /**
   * Gets called whenever an entity is created via the render system.
   * @param e scene graph entity, that already contains a generic Renderable component.
   */
  inline void onEntityCreate(SceneGraphEntity& e) {
    const auto& renderable = e.get<izz::Geometry>();
    e.add(DeferredRenderable{.materialId = renderable.materialId, .vertexBufferId = renderable.vertexBufferId});
  }

  void setClearColor(const glm::vec4 color);

 private:
  void renderGeometryPass(const entt::registry& registry);
  void renderLightingPass();

  std::shared_ptr<MaterialSystem> m_materialSystem;
  std::shared_ptr<MeshSystem> m_meshSystem;
  entt::registry& m_registry;

  glm::vec4 m_clearColor;
  GLuint m_gBufferFbo, m_lightingPassFbo;

  /// Texture id's (obtained via glGenTextures)
  GLuint m_gPosition, m_gNormal, m_gTangent, m_gAlbedoSpec;
  GLuint m_depthBuffer;

  /// Uniform locations in shader (obtained via glGetUniformLocation).
  GLint m_gPositionLoc = -1, m_gNormalLoc = -1, m_gAlbedoSpecLoc = -1;
  entt::entity m_activeCamera = entt::null;

  int m_screenWidth;
  int m_screenHeight;

  VertexBufferId m_screenSpaceMeshBufferId{-1};
  MaterialId m_lightPassMaterialId{-1};
};

}  // namespace gl
}  // namespace izz
