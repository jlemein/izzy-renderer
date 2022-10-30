//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <izzgui_iguiwindow.h>
#include <memory>
#include "izzgui_shadereditor.h"

namespace izz {
class EntityFactory;

namespace gl {
class MaterialSystem;
}

namespace gui {

class MaterialEditor : public gui::IGuiWindow {
 public:
  MaterialEditor(std::shared_ptr<gl::MaterialSystem> materialSystem, std::shared_ptr<izz::EntityFactory> sceneGraph);

  static inline const char* ID{"Material editor"};

  void init() override;
  void render(float time, float dt) override;
  static void Open();
  static void Toggle();

 private:
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  ShaderEditor m_shaderEditor;
  std::shared_ptr<izz::EntityFactory> m_sceneGraph;

  static inline bool Show{false};

  void drawMaterialTable();
  void drawShaderProgramTable();
};

}  // namespace gui
}  // namespace izz
