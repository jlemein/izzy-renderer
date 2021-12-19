//
// Created by jlemein on 09-08-21.
//
#ifndef RENDERER_ANIM_ANIMATIONSYSTEM_H
#define RENDERER_ANIM_ANIMATIONSYSTEM_H

#include <memory>

namespace izz {
class SceneGraph;
}  // namespace izz

namespace lsw {
namespace anim {

class AnimationSystem {  //: public ecs::IViewerExtension {
 public:
  AnimationSystem(std::shared_ptr<izz::SceneGraph> sceneGraph);

  void init();
  void update(float time, float dt);

 private:
  std::shared_ptr<izz::SceneGraph> m_sceneGraph;
};

}  // namespace anim
}  // namespace lsw

#endif  // RENDERER_ANIM_ANIMATIONSYSTEM_H
