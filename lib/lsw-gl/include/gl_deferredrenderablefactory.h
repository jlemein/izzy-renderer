//
// Created by jlemein on 18-02-22.
//
#pragma once

namespace lsw {
namespace geo {
struct Material;
}
}
namespace izz {
namespace gl {

struct DeferredRenderable;
class RenderSystem;

/**
 * Factory creation methods
 */
class DeferredRenderableFactory {
 public:
  /**
   * Creates a deferred renderable entity filled with
   * @param material
   * @param renderSystem
   * @return
   */
  static DeferredRenderable CreateRenderable(const lsw::geo::Material& material, RenderSystem& renderSystem);
};
}
}
