#include <ecs_transformsystem.h>

#include <ecs_transform.h>

#include <entt/entt.hpp>
#include <ecs_relationship.h>
#include <deque>

using namespace artifax::ecs;

//
// Created by jlemein on 10-11-20.
//
TransformSystem::TransformSystem(entt::registry &registry) : m_registry{registry} {}

void
TransformSystem::init()
{
  //  registry.on_construct<Transform>().connect<&TransformSystem::onConstruct>();
}

void
TransformSystem::onConstruct(entt::entity e)
{
}


void
TransformSystem::update(float time, float dt)
{
  auto viewe = m_registry.view<Transform>();
  for (auto e : viewe) {
    auto &transform = viewe.get<Transform>(e);
    transform.worldTransform = transform.localTransform;
  }

  auto view = m_registry.view<Transform, Relationship>();

  for (auto e : view) {
    // only handle parent entities
    const auto &rel = view.get<Relationship>(e);
    if (rel.parent != entt::null)
      continue;

    auto &parentTransform = view.get<Transform>(e);

    // initially parent sets world transform equal to local transform
    parentTransform.worldTransform = parentTransform.localTransform;

    std::deque<entt::entity> stack;
    std::deque<entt::entity> parentStack;

    if (rel.children.size() > 0) {
      parentStack.emplace_back(e);
      for (auto c : rel.children)
        stack.emplace_back(c);
    }

    // for current parent
    while (!stack.empty()) {
      auto child = stack.front();
      auto parent = stack.front();

      auto &parentTransform = view.get<Transform>(parent);

      auto transformChild = m_registry.get<Transform>(child);
      transformChild.worldTransform = parentTransform.worldTransform * transformChild.localTransform;

      auto relChild = m_registry.try_get<Relationship>(child);
      if (relChild != nullptr && relChild->children.size() > 0) {
        parentStack.emplace_back(child);
        for (auto c : relChild->children)
          stack.emplace_back(c);
      }

      stack.pop_front();
      parentStack.pop_front();
    };
  }

//
//  m_registry.sort<Relationship>([this](const entt::entity lhs, const entt::entity rhs) {
//
//  });
}


void
TransformSystem::render()
{
}
