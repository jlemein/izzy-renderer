#include <ecs_transformsystem.h>

#include <ecs_transform.h>

#include <deque>
#include <ecs_dirty.h>
#include <ecs_relationship.h>
#include <ecs_renderable.h>
#include <entt/entt.hpp>

using namespace affx::ecs;

namespace {

void sortTransforms(entt::registry &registry) {
  registry.sort<Relationship>(
      [&registry](const entt::entity lhs, const entt::entity rhs) {
        const auto &clhs = registry.get<Relationship>(lhs);
        const auto &crhs = registry.get<Relationship>(rhs);

        // if true, then lhs denotes a smaller (or equal) value than rhs
        return crhs.parent == lhs || lhs <= rhs;
      });
}
} // namespace

//
// Created by jlemein on 10-11-20.
//
TransformSystem::TransformSystem(entt::registry &registry)
    : m_registry{registry} {}

void TransformSystem::init() {
  //  registry.on_construct<Transform>().connect<&TransformSystem::onConstruct>();

  // sort the transforms, starting with root, followed by
  //  m_registry.sort<Relationship>
}

void TransformSystem::onConstruct(entt::entity e) {}

namespace {
std::vector<entt::entity> getRootNodes(entt::registry &registry) {
  std::vector<entt::entity> rootEntities;

  auto view = registry.view<Transform>();
  for (auto e : view) {
    if (registry.has<Relationship>(e) ||
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
  // assume that the graph is sorted in breadth first
  auto view = m_registry.view<Transform>();
  for (auto e : view) {
    auto &transform = view.get<Transform>(e);
    transform.worldTransform = transform.localTransform;
  }

  auto view2 = m_registry.view<Transform, Relationship>();
  for (auto e : view2) {
    auto relationship = m_registry.get<Relationship>(e);

    if (relationship.parent != entt::null) {
      const auto &parentTransform =
          m_registry.get<Transform>(relationship.parent);

      // update transform with
      auto &transform = view.get<Transform>(e);
      transform.worldTransform =
          parentTransform.worldTransform * transform.localTransform;
    }
  }
}

void TransformSystem::render() {}
