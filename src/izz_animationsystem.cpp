//
// Created by jlemein on 09-08-21.
//
#include "../include/anim_animationsystem.h"
#include "../include/anim_localrotation.h"
#include "glrs_rendersubsystem.h"
#include "ecs_transform.h"
#include "ecsg_scenegraph.h"
#include <iostream>
using namespace lsw::anim;
using namespace lsw::ecs;

AnimationSystem::AnimationSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph)
: m_sceneGraph{sceneGraph}
{}

void AnimationSystem::init() {}

void AnimationSystem::update(float time, float dt) {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<anim::LocalRotation, Transform>();

  for (auto e : view) {
    auto &t = registry.get<Transform>(e);
    auto &rot = view.get<LocalRotation>(e);

    // TODO: make quaternion operation
    t.localTransform = glm::rotate(glm::mat4(1.F), rot.radiansPerSecond*dt, rot.axis) * t.localTransform;
  }
}

