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

class SceneNavigator : public gui::IGuiWindow {
 public:
  static inline const char* ID = "Navigator";

  static void Open();
  static void Toggle();

  SceneNavigator(std::shared_ptr<izz::EntityFactory> sceneGraph);

  void init() override;
  void render(float time, float dt) override;

 private:
  static inline bool Show = true;

  std::shared_ptr<izz::EntityFactory> m_sceneGraph;

  entt::entity m_selectedHierarchyNode = entt::null;
  ImRect renderHierarchy(entt::registry& reg, entt::entity e, int depth = 0);

  const Icon* getIconForEntity(const entt::registry& registry, entt::entity e);
};

}  // namespace gui
}  // namespace izz
