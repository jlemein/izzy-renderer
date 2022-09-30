//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>
#include "gl_renderable.h"
#include "izz.h"
#include "izz_scenegraphentity.h"

namespace izz {
namespace gl {

class Material;
class MaterialSystem;
class MeshSystem;

struct ForwardRenderable {
  bool isWireframe{false};
  MaterialId materialId{-1};
  MeshBufferId meshBufferId{-1};
};

class ForwardRenderer {
  static constexpr inline const char* ID = "ForwardRenderer";

 private:
  std::shared_ptr<MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<MeshSystem> m_meshSystem{nullptr};
  entt::registry& m_registry;

 public:
  ForwardRenderer(std::shared_ptr<MaterialSystem> materialSystem, std::shared_ptr<MeshSystem> meshSystem, entt::registry& registry);

  /**
   * @brief Called when an gl::DeferredRenderable component is added to the entity.
   * Used to initialize the underlying data structures.
   */
  void onConstruct(entt::registry& r, entt::entity e);

  void render(const entt::registry& registry);

  void update(float dt, float time);

  /**
   * Gets called whenever an entity is created via the render system.
   * @param e scene graph entity, that already contains a generic Renderable component.
   */
  inline void onEntityCreate(SceneGraphEntity& e) {
    auto& renderable = e.get<gl::Renderable>();
    e.add(ForwardRenderable{.materialId = renderable.materialId, .meshBufferId = renderable.materialId});
  }
};

}  // namespace gl
}  // namespace izz
