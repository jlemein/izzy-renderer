//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <izzgl_material.h>
#include <entt/fwd.hpp>
#include "gl_renderable.h"
#include "izz.h"
#include "izz_scenegraphentity.h"
#include <izzgl_depthpeeling.h>

namespace izz {
namespace gl {

class MaterialSystem;
class TextureSystem;
class MeshSystem;

struct ForwardRenderable {
  MaterialId materialId{-1};
  VertexBufferId vertexBufferId{-1};
  bool isWireframe{false};
  izz::geo::BlendMode blendMode { izz::geo::BlendMode::OPAQUE };
};

class ForwardRenderer {
  static constexpr inline const char* ID = "ForwardRenderer";

 private:
  std::shared_ptr<MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<MeshSystem> m_meshSystem{nullptr};
  entt::registry& m_registry;
  DepthPeeling m_depthPeeling;
  bool m_isAlphaBlendingEnabled = true;
  int m_width, m_height;


 public:
  ForwardRenderer(std::shared_ptr<MaterialSystem> materialSystem,
                  std::shared_ptr<TextureSystem> textureSystem,
                  std::shared_ptr<MeshSystem> meshSystem, entt::registry& registry);

  /**
   * @brief Called when an gl::DeferredRenderable component is added to the entity.
   * Used to initialize the underlying data structures.
   */
  void onConstruct(entt::registry& r, entt::entity e);

  /**
   * Initializes the forward renderer with buffers and materials it needs (such as frame buffers).
   * @param width   Width of the frame buffer.
   * @param height  Height of the frame buffer.
   */
  void init(int width, int height);

  void render(const entt::registry& registry);

  void update(float dt, float time);

  /**
   * Gets called whenever an entity is created via the render system.
   * @param e scene graph entity, that already contains a generic Renderable component.
   */
  void onEntityCreate(SceneGraphEntity& e);

  void setClearColor(const glm::vec4& color);

 private:
  void renderOpaqueObjects(const entt::registry& registry);

  glm::vec4 m_clearColor;
};

}  // namespace gl
}  // namespace izz
