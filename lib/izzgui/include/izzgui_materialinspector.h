//
// Created by jeffrey on 11-11-22.
//

#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <izzgui_iguiwindow.h>
#include <izzgui_imguix.h>
#include <memory>
#include "izzgui_shadereditor.h"
#include <izz.h>

namespace izz {
class EntityFactory;

namespace gl {
class MaterialSystem;
}

namespace gui {

/**
 * Inspector shows the defined materials. It visualizes the materials.json file.
 */
class MaterialInspector : public gui::IGuiWindow {
 public:
  MaterialInspector(std::shared_ptr<gl::MaterialSystem> materialSystem);

  /// Opens the material inspector.
  static void Open();
  static void Toggle();
  static MaterialId SelectedMaterial; //= MATERIAL_UNDEFINED;

  static inline const char* ID = "Material Inspector";

  void init() override;
  void render(float time, float dt) override;
  void open();

 private:
  static bool inline Show = true;  /// controls whether the window is visible/displayed.
  int m_guiId = 0;
  float m_indent = 10.0F;
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::EntityFactory> m_sceneGraph {nullptr};
  ImVec4 m_frameHeaderColor = ImVec4(ImColor(0, 0x40, 0));            /// Color used for background of component headers.
  ImVec4 m_inputBackgroundColor = ImVec4(ImColor(0x20, 0x20, 0x20));  /// Background color for input controls.

  void generalInfo();
  void uniformProperties();
  void compileConstants();
};

}  // namespace gui
}  // namespace izz

