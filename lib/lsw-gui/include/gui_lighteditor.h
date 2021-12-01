#pragma once

#include <ecsg_scenegraph.h>
#include <imgui.h>
#include <izz_fontsystem.h>
#include <entt/entt.hpp>
#include <memory>
#include "gui_guiwindow.h"

namespace lsw {

class ResourceManager;

namespace gui {

class ResourceInspector : public IGuiWindow {
 public:
  ResourceInspector(std::shared_ptr<lsw::ResourceManager> manager);

  void init() override;

  void render(float time, float dt) override;

 private:
  std::shared_ptr<lsw::ResourceManager> m_resourceManager{nullptr};
};
// Multiple GUI windows are needed
// * Scene hierarchy view
// * Resource inspector (i.e. textures, videos, sounds, images)
class GuiLightEditor : public IGuiWindow {
 public:
  GuiLightEditor(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<lsw::FontSystem> fontSystem);

  void init() override;
  void render(float dt, float totalTime) override;

 private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  std::shared_ptr<lsw::FontSystem> m_fontSystem;

  std::vector<ImFont*> m_fonts;

  entt::registry& m_registry;

  std::string m_selectedTexture{""};
};
}  // namespace gui
}  // namespace lsw