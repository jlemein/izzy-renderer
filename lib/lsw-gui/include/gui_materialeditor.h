//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <gui_iguiwindow.h>
#include <memory>
#include "gui_shadereditor.h"

namespace izz {
class SceneGraph;

namespace gui {

class MaterialEditor : public gui::IGuiWindow {
 public:
  MaterialEditor(std::shared_ptr<izz::SceneGraph> sceneGraph);

  void init() override;
  void render(float time, float dt) override;
  void open();

 private:
  ShaderEditor m_shaderEditor;
  std::shared_ptr<izz::SceneGraph> m_sceneGraph;
  bool m_show{false};
};

}  // namespace gui
}  // namespace izz
