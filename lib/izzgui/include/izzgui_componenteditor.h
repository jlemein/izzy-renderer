//
// Created by jeffrey on 11-11-22.
//
#pragma once

#include <imgui.h>
#include <izzgui_iguiwindow.h>
#include <entt/fwd.hpp>
#include <memory>

namespace izz {

class Izzy;

namespace gui {

class ComponentEditor : public IGuiWindow {
 public:
  static inline const char* ID = "Component Editor";
  static entt::entity SelectedEntity;
  static inline bool Show = true;

  static void Open();
  static void Toggle();

  ComponentEditor(izz::Izzy& izzy);

  void init() override;

  void render(float time, float dt) override;

 private:
  izz::Izzy& m_izzy;
  ImVec4 m_frameHeaderColor = ImVec4(ImColor(0x40, 0, 0));            /// Color used for background of component headers.
  ImVec4 m_inputBackgroundColor = ImVec4(ImColor(0x20, 0x20, 0x20));  /// Background color for input controls.

  void addComponentMenuBar();

  // components
  void transformComponent();
  void nameComponent();
  void cameraComponent();
  void fpsControlComponent();
  void directionalLightComponent();
  void pointLightComponent();
  void spotLightComponent();
  void ambientLightComponent();
};

}  // namespace gui
}  // namespace izz
