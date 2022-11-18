//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <izz.h>
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
class Material;
}

namespace gui {

class MaterialNavigator : public gui::IGuiWindow {
 public:
  static inline const char* ID = "Material Navigator";

  static void Open();
  static void Toggle();

  MaterialNavigator(std::shared_ptr<izz::EntityFactory> sceneGraph, std::shared_ptr<izz::gl::MaterialSystem> materialSystem);

  void init() override;
  void render(float time, float dt) override;

 private:
  static inline bool Show = true;

  std::shared_ptr<izz::EntityFactory> m_sceneGraph{nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};

  MaterialId m_selectedMaterial = izz::MATERIAL_UNDEFINED;
  ImRect renderHierarchy(izz::gl::Material& material, int depth = 0);
};

}  // namespace gui
}  // namespace izz
