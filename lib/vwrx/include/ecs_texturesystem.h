//
// Created by jlemein on 06-11-20.
//

#ifndef GLVIEWER_VIEWER_TEXTURESYSTEM_H
#define GLVIEWER_VIEWER_TEXTURESYSTEM_H

#include <ecs_rendersubsystem.h>
#include <vwr_viewerextension.h>
#include <entt/entt.hpp>
#include <memory>

namespace affx {
namespace viewer {
class Viewer;
}

namespace ecs {
class SceneGraph;

class TextureSystem : public affx::ecs::IViewerExtension,
                      public affx::ecs::IRenderSubsystem {
public:
  TextureSystem(std::shared_ptr<viewer::Viewer> viewer);
  virtual ~TextureSystem() override = default;

  // @see IViewerExtension
  void initialize() override;
    // converts geo::Material components into respective entities having an
    // ecs::Texture component.

  void update(float time, float dt) override;
  void beforeRender() override {}
  void afterRender() override {}
  void cleanup() override {}

  // @see IRenderSubsystem
  void onRender(const entt::registry &registry, entt::entity entity) override;

private:
  ecs::SceneGraph& m_sceneGraph;
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_VIEWER_TEXTURESYSTEM_H
