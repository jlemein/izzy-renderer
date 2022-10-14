#pragma once

#include <imgui.h>
#include <izz_fontsystem.h>
#include <entt/entt.hpp>
#include <memory>
#include "gui_iguiwindow.h"
#include <izzgl_entityfactory.h>

namespace izz {
class ResourceManager;
namespace gui {

class LightEditor : public IGuiWindow {
 public:
  LightEditor(std::shared_ptr<izz::EntityFactory> sceneGraph, std::shared_ptr<izz::FontSystem> fontSystem);

  void init() override;
  void render(float dt, float totalTime) override;

  static inline bool Show = false;

 private:
  std::shared_ptr<izz::EntityFactory> m_sceneGraph;
  std::shared_ptr<izz::FontSystem> m_fontSystem;

  std::vector<ImFont*> m_fonts;

  entt::registry& m_registry;

  std::string m_selectedTexture{""};
};
}  // namespace gui
}  // namespace izz