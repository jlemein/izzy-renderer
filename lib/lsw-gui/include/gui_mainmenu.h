//
// Created by jeffrey on 12-12-21.
//
#pragma once

#include <gui_iguiwindow.h>
#include <memory>
#include "gui_materialeditor.h"

namespace lsw {
namespace ecsg {
class SceneGraph;
}  // namespace ecsg
}  // namespace lsw

namespace izz {
namespace gui {

class MainMenu : public IGuiWindow {
 public:
  MainMenu(std::shared_ptr<lsw::ecsg::SceneGraph> sceneGraph);

  void init() override;
  void render(float time, float dt) override;

 private:
  std::shared_ptr<lsw::ecsg::SceneGraph> m_sceneGraph;

  gui::MaterialEditor m_materialEditor;
};
}  // namespace gui
}  // namespace izz
