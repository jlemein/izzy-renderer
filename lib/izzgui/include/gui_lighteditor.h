#pragma once

#include "izz_scenegraphhelper.h"
#include <imgui.h>
#include <izz_fontsystem.h>
#include <entt/entt.hpp>
#include <memory>
#include "gui_iguiwindow.h"

namespace izz {
class ResourceManager;
namespace gui {

class LightEditor : public IGuiWindow {
 public:
  LightEditor(std::shared_ptr<izz::SceneGraphHelper> sceneGraph, std::shared_ptr<izz::FontSystem> fontSystem);

  void init() override;
  void render(float dt, float totalTime) override;

  static inline bool Show = false;

 private:
  std::shared_ptr<izz::SceneGraphHelper> m_sceneGraph;
  std::shared_ptr<izz::FontSystem> m_fontSystem;

  std::vector<ImFont*> m_fonts;

  entt::registry& m_registry;

  std::string m_selectedTexture{""};
};
}  // namespace gui
}  // namespace izz