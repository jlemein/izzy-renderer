//
// Created by jeffrey on 12-12-21.
//
#pragma once
#include <gui_iguiwindow.h>
#include <memory>

namespace lsw {
namespace ecsg {
class SceneGraph;
}  // namespace ecsg
}  // namespace lsw

namespace izz {
namespace gui {

class MaterialEditor : public gui::IGuiWindow {
 public:
  MaterialEditor(std::shared_ptr<lsw::ecsg::SceneGraph> sceneGraph);

  void init() override;
  void render(float time, float dt) override;
  void open();

 private:
  std::shared_ptr<lsw::ecsg::SceneGraph> m_sceneGraph;
  bool m_show{false};
};

}  // namespace gui
}  // namespace izz
