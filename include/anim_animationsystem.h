//
// Created by jlemein on 09-08-21.
//
#pragma once

#include <memory>
#include <entt/entity/registry.hpp>

namespace izz {
class SceneGraphHelper;
}  // namespace izz

namespace izz {
namespace anim {

class AnimationSystem {  //: public ecs::IViewerExtension {
 public:
  AnimationSystem(entt::registry& registry);

  void init();
  void update(float time, float dt);

 private:
  entt::registry& m_registry;
};

}  // namespace anim
}  // namespace lsw

