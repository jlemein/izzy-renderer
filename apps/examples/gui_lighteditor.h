#pragma once

#include <ecsg_scenegraph.h>
#include <entt/entt.hpp>
#include <memory>

namespace lsw {
namespace gui {

class GuiLightEditor {
 public:
  GuiLightEditor(std::shared_ptr<ecsg::SceneGraph> sceneGraph);
  void operator()(float dt, float totalTime);

 private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  entt::registry& m_registry;
};
}
}