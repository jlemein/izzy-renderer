//
// Created by jlemein on 09-08-21.
//
#ifndef RENDERER_ANIM_ANIMATIONSYSTEM_H
#define RENDERER_ANIM_ANIMATIONSYSTEM_H

#include <memory>

namespace lsw {
namespace ecsg {
class SceneGraph;
}

namespace anim {

class AnimationSystem { //: public ecs::IViewerExtension {
public:
  AnimationSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph);

  void init();
  void update(float time, float dt);

private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
};

} // namespace anim
} // namespace lsw

#endif // RENDERER_ANIM_ANIMATIONSYSTEM_H
