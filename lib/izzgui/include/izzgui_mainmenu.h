//
// Created by jeffrey on 12-12-21.
//
#pragma once

#include <izzgui_iguiwindow.h>
#include <izzgui_materialeditor.h>
#include <izzgui_materialinspector.h>
#include <izzgui_scenenavigator.h>
#include <memory>

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
};
}  // namespace gui
}  // namespace izz
