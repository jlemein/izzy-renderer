//
// Created by jlemein on 09-08-21.
//
#pragma once

#include <memory>

namespace izz {
class SceneGraphHelper;
}  // namespace izz

namespace lsw {
namespace anim {

class AnimationSystem {  //: public ecs::IViewerExtension {
 public:
  AnimationSystem(std::shared_ptr<izz::SceneGraphHelper> sceneGraph);

  void init();
  void update(float time, float dt);

 private:
  std::shared_ptr<izz::SceneGraphHelper> m_sceneGraph;
};

}  // namespace anim
}  // namespace lsw

