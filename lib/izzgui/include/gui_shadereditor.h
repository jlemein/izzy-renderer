//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <gui_iguiwindow.h>
#include "izz_scenegraphentity.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace izz {

class EntityFactory;

namespace gui {

class ShaderEditor : public IGuiWindow {
 public:
  ShaderEditor(std::shared_ptr<izz::EntityFactory> sceneGraph);

  void init() override;
  void render(float time, float dt) override;

  void openDialog(entt::entity entity);

 private:
  std::shared_ptr<izz::EntityFactory> m_sceneGraph;
  std::unordered_map<entt::entity, bool> m_openDialogs;
};

}  // namespace gui
}  // namespace izz