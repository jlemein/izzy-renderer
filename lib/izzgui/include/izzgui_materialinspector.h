//
// Created by jeffrey on 11-11-22.
//

#pragma once

//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <izzgui_iguiwindow.h>
#include <izzgui_imguix.h>
#include <memory>
#include "izzgui_shadereditor.h"

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

  static inline const char* ID = "Material inspector";

  void init() override;
  void render(float time, float dt) override;
  void open();

 private:
  static bool inline Show = false;  /// controls whether the window is visible/displayed.
  int m_guiId = 0;
  float m_indent = 10.0F;
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<izz::EntityFactory> m_sceneGraph {nullptr};
};

}  // namespace gui
}  // namespace izz

