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
  static DeferredRenderable CreateRenderable(const lsw::geo::Material& material, RenderSystem& renderSystem);
};
}
}
