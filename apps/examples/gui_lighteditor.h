#pragma once

#include <ecsg_scenegraph.h>
#include <entt/entt.hpp>
#include <memory>
#include <georm_fontsystem.h>
#include <gui_system.h>
#include <imgui.h>

namespace lsw {
namespace gui {

class GuiLightEditor : public IGuiWindow {
 public:
  GuiLightEditor(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<georm::FontSystem> fontSystem);

  void init() override;
  void render(float dt, float totalTime) override;

 private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  std::shared_ptr<georm::FontSystem> m_fontSystem;

  std::vector<ImFont*> m_fonts;

  entt::registry& m_registry;
};
}
}