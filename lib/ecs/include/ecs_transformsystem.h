//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_ECS_TRANSFORMSYSTEM_H
#define GLVIEWER_ECS_TRANSFORMSYSTEM_H

//#include <viewer_interactable.h>
#include <entt/entt.hpp>

namespace affx {
namespace ecs {

class TransformSystem { //: public IViewerInteractable {
public:
  TransformSystem(entt::registry& registry);

  void init();
  void update(float time, float dt);
  void render();

 private:
  void onConstruct(entt::entity e);

//  entt::observer m_observer;
  entt::registry& m_registry;
};

} // end package namespace
} // end enterprise namesapce

#endif // GLVIEWER_ECS_TRANSFORMSYSTEM_H
