//
// Created by jlemein on 01-02-21.
//
#include <izz.h>
#include <ecs_transform.h>
#include <ecs_transformutil.h>
#include <izz_scenegraphentity.h>
#include <izz_relationshiputil.h>
using namespace izz;

SceneGraphEntity::SceneGraphEntity(entt::registry& registry, entt::entity handle)
  : m_registry{registry}
  , m_handle{handle} {}

uint64_t SceneGraphEntity::id() const {
  return static_cast<uint64_t>(m_handle);
}

entt::entity SceneGraphEntity::handle() {
  return m_handle;
}

std::string SceneGraphEntity::getName() const {
  return m_registry.get<Name>(m_handle).name;
}

void SceneGraphEntity::setName(std::string name) {
  m_registry.emplace_or_replace<Name>(m_handle, Name{name});
}

glm::mat4& SceneGraphEntity::getTransform() {
  return m_registry.get<ecs::Transform>(m_handle).localTransform;
}

const glm::mat4& SceneGraphEntity::getTransform() const {
  return m_registry.get<ecs::Transform>(m_handle).localTransform;
}

void SceneGraphEntity::setTransform(const glm::mat4& transform) {
  m_registry.get<ecs::Transform>(m_handle).localTransform = transform;
}

void SceneGraphEntity::translate(const glm::vec3& translate) {
  ecs::TransformUtil::Translate(this->get<ecs::Transform>(), translate);
}

void SceneGraphEntity::addChild(SceneGraphEntity child) {
  RelationshipUtil::MakeChild(m_registry, m_handle, child.handle());
}
