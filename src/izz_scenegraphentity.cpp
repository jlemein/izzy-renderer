//
// Created by jlemein on 01-02-21.
//
#include <ecs_transform.h>
#include <ecs_transformutil.h>
#include <izz.h>
#include <izz_relationshiputil.h>
#include <izz_scenegraphentity.h>
#include "izz_relationship.h"
using namespace izz;

SceneGraphEntity::SceneGraphEntity(entt::registry& registry, entt::entity handle)
  : m_registry{&registry}
  , m_handle{handle} {}

SceneGraphEntity::SceneGraphEntity(const SceneGraphEntity& other)
  : m_registry{other.m_registry}
  , m_handle{other.m_handle} {}

SceneGraphEntity& SceneGraphEntity::operator=(const SceneGraphEntity& other) {
  this->m_registry = other.m_registry;
  this->m_handle = other.m_handle;
  return *this;
}

uint64_t SceneGraphEntity::id() const {
  return static_cast<uint64_t>(m_handle);
}

entt::entity SceneGraphEntity::handle() const {
  return m_handle;
}

std::string SceneGraphEntity::getName() const {
  return m_registry->get<Name>(m_handle).name;
}

void SceneGraphEntity::setName(std::string name) {
  m_registry->emplace_or_replace<Name>(m_handle, Name{name});
}

glm::mat4& SceneGraphEntity::getTransform() {
  return m_registry->get<ecs::Transform>(m_handle).localTransform;
}

const glm::mat4& SceneGraphEntity::getWorldTransform() {
  return m_registry->get<ecs::Transform>(m_handle).worldTransform;
}

const glm::mat4& SceneGraphEntity::getTransform() const {
  return m_registry->get<ecs::Transform>(m_handle).localTransform;
}

void SceneGraphEntity::setTransform(const glm::mat4& transform) {
  m_registry->get<ecs::Transform>(m_handle).localTransform = transform;
}

SceneGraphEntity SceneGraphEntity::getParent() const {
  auto& relationship = m_registry->get<Relationship>(m_handle);
  return SceneGraphEntity{*m_registry, relationship.parent};
}

bool SceneGraphEntity::exists() const {
  return m_handle != entt::null;
}

void SceneGraphEntity::translate(const glm::vec3& translate) {
  ecs::TransformUtil::Translate(this->get<ecs::Transform>(), translate);
}

void SceneGraphEntity::addChild(SceneGraphEntity child) {
  RelationshipUtil::MakeChild(*m_registry, m_handle, child.handle());
}

std::vector<SceneGraphEntity> SceneGraphEntity::getChildren() {
  auto& relationship = m_registry->get<Relationship>(m_handle);
  std::vector<SceneGraphEntity> children{};
  children.reserve(relationship.children.size());

  std::transform(relationship.children.begin(), relationship.children.end(), std::back_inserter(children),
            [r = m_registry](entt::entity e) {
    return SceneGraphEntity{*r, e};
  });
  return children;
}