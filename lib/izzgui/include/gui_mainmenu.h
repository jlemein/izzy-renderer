//
// Created by jeffrey on 12-12-21.
//
#pragma once

#include <gui_iguiwindow.h>
#include <memory>
#include "gui_materialeditor.h"

namespace izz {
class Izzy;
class EntityFactory;
namespace gui {

class MainMenu : public IGuiWindow {
 public:
  MainMenu(Izzy& izzy);

  void init() override;
  void render(float time, float dt) override;

 private:
  std::shared_ptr<izz::EntityFactory> m_sceneGraph;

  gui::MaterialEditor m_materialEditor;
};
}  // namespace gui
}  // namespace izz
