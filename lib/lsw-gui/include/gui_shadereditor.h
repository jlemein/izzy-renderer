//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <gui_iguiwindow.h>
#include <ecsg_scenegraphentity.h>

#include <memory>
#include <vector>
#include <unordered_map>

namespace lsw {
namespace ecsg {
class SceneGraph;
}
}

namespace izz {
namespace gui {

class ShaderEditor : public IGuiWindow {
 public:
  ShaderEditor(std::shared_ptr<lsw::ecsg::SceneGraph> sceneGraph);

  void init() override;
  void render(float time, float dt) override;

  void openDialog(entt::entity entity);

 private:
  std::shared_ptr<lsw::ecsg::SceneGraph> m_sceneGraph;
  std::unordered_map<entt::entity, bool> m_openDialogs;
};

}  // namespace gui
}  // namespace izz