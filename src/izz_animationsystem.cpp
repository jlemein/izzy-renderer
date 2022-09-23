//
// Created by jlemein on 09-08-21.
//
#include <ecs_transform.h>
#include "anim_animationsystem.h"
#include "anim_localrotation.h"
#include "izz_entityfactory.h"
using namespace izz::anim;
using namespace izz::ecs;

AnimationSystem::AnimationSystem(entt::registry& registry)
: m_registry{registry}
{}

void AnimationSystem::init() {}

void AnimationSystem::update(float time, float dt) {
  auto view = m_registry.view<anim::LocalRotation, Transform>();

  for (auto e : view) {
    auto &t = m_registry.get<Transform>(e);
    auto &rot = view.get<LocalRotation>(e);

    // TODO: make quaternion operation
    t.localTransform = glm::rotate(glm::mat4(1.F), rot.radiansPerSecond*dt, rot.axis) * t.localTransform;
  }
}

