//
// Created by jlemein on 18-02-22.
//
#pragma once

#include <izz_renderablecomponentfactory.h>

namespace lsw {
namespace geo {
struct Material;
}
}
namespace izz {
namespace gl {

struct DeferredRenderable;
class RenderSystem;
class MaterialSystem;
/**
 * Factory creation methods
 */
class DeferredRenderableFactory : public RenderableComponentFactory {
 public:
  DeferredRenderableFactory(RenderSystem& renderSystem, MaterialSystem& materialSystem);

  /**
   * Creates a deferred renderable entity filled with
   * @param material
   * @param renderSystem
   * @return
   */
  void addRenderableComponent(entt::registry& registry, entt::entity e, int materialId, int meshBufferId) override;

 private:
  RenderSystem& m_renderSystem;
  MaterialSystem& m_materialSystem;
};
}
}
