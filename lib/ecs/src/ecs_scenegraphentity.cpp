//
// Created by jlemein on 01-02-21.
//
#include <ecs_scenegraphentity.h>
#include <ecs_relationship.h>
#include <ecs_relationshiputil.h>
#include <ecs_transform.h>
using namespace affx::ecs;

SceneGraphEntity::SceneGraphEntity(entt::registry& registry, entt::entity handle)
  : m_registry{registry}
  , m_handle {handle} {}

entt::entity SceneGraphEntity::id() {
  return m_handle;
}

entt::entity SceneGraphEntity::handle() {
  return m_handle;
}

void SceneGraphEntity::addChild(SceneGraphEntity child) {
  RelationshipUtil::MakeChild(m_registry, m_handle, child.handle());
}

void SceneGraphEntity::setTransform(glm::mat4 transform) {
  m_registry.get<Transform>(m_handle).localTransform = transform;
}
