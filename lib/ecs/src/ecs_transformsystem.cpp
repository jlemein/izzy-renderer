#include <ecs_transformsystem.h>

#include <ecs_transform.h>

#include <ecs_relationship.h>
#include <ecsu_relationshipsorter.h>
#include <entt/entt.hpp>

using namespace lsw::ecs;

//
// Created by jlemein on 10-11-20.
//
TransformSystem::TransformSystem(entt::registry &registry)
    : m_registry{registry} {}

void TransformSystem::init() {
  // sort the transforms, so that the root transform is first, followed by
  // children of the root, followed by children of their children, etc.
  // leaf transforms are to the right of the transform system.

  ecsu::RelationshipSorter::Sort<Transform>(m_registry);
}

namespace {
std::vector<entt::entity> getRootNodes(entt::registry &registry) {
  std::vector<entt::entity> rootEntities;

  auto view = registry.view<Transform>();
  for (auto e : view) {
    if (registry.all_of<Relationship>(e) ||
        registry.get<Relationship>(e).parent == entt::null) {
      rootEntities.push_back(e);
    }
  }
  return rootEntities;
}

} // namespace

void updateChildTransform(entt::registry &registry,
                          std::vector<entt::entity> &children,
                          Transform &parentTransform) {
  // update children recursively
  for (auto child : children) {
    //      updateChildTransform(child, transform);
    auto &transform = registry.get<Transform>(child);
    transform.worldTransform = parentTransform.worldTransform;
  }
}

void TransformSystem::update(float time, float dt) {

  auto view = m_registry.view<Transform, Relationship>();
  for (auto e : view) {
    auto relationship = m_registry.get<Relationship>(e);
    auto &transform = view.get<Transform>(e);

    if (relationship.parent == entt::null) {
      transform.worldTransform = transform.localTransform;
    }
    else
    {
      const auto &parentTransform =
          m_registry.get<Transform>(relationship.parent);

      auto &transform = view.get<Transform>(e);
      transform.worldTransform =
          parentTransform.worldTransform * transform.localTransform;
    }
  }
}

void TransformSystem::render() {}
