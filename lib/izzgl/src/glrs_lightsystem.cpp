#include <ecs_light.h>
#include <ecs_transform.h>
#include <glrs_lightsystem.h>
#include <izz_behavior.h>
#include <izz_izzy.h>
#include <izz_relationship.h>
#include <izzgl_entityfactory.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <memory>
using namespace izz;
using namespace izz::gl;

LightSystem::LightSystem(entt::registry& registry, MaterialSystem& materialSystem, MeshSystem& meshSystem)
  : m_registry{registry}
  , m_materialSystem{materialSystem}
  , m_meshSystem{meshSystem} {
  m_registry.on_construct<ecs::PointLightTracker>().connect<&LightSystem::onPointLightTrackerAdded>(this);
  m_registry.on_construct<ecs::SpotLightTracker>().connect<&LightSystem::onSpotLightTrackerAdded>(this);
  m_registry.on_construct<ecs::DirectionalLightTracker>().connect<&LightSystem::onDirectionalLightTrackerAdded>(this);
}

void LightSystem::setRenderSystem(izz::gl::RenderSystem& renderSystem) {
  m_entityFactory = std::make_unique<EntityFactory>(m_registry, renderSystem, m_materialSystem, m_meshSystem);
}

int LightSystem::getActiveLightCount() const {
  auto pointLights = m_registry.view<izz::ecs::Transform, izz::ecs::PointLight>();
  auto dirLights = m_registry.view<izz::ecs::Transform, izz::ecs::DirectionalLight>();
  auto ambientLights = m_registry.view<izz::ecs::AmbientLight>();

  return pointLights.size_hint() + dirLights.size_hint() + ambientLights.size();
}

void LightSystem::setDefaultPointLightMaterial(int materialId) {
  m_lightMaterial = materialId;
}

void LightSystem::onPointLightTrackerAdded(entt::registry& r, entt::entity e) {
  auto& tracker = r.get<izz::ecs::PointLightTracker>(e);
  if (tracker.light == entt::null) {
    tracker.light = r.all_of<ecs::PointLight>(e) ? e : r.get<Relationship>(e).parent;
  }
}

void LightSystem::onSpotLightTrackerAdded(entt::registry& r, entt::entity e) {
  auto& tracker = r.get<izz::ecs::SpotLightTracker>(e);
  if (tracker.light == entt::null) {
    tracker.light = r.all_of<ecs::SpotLight>(e) ? e : r.get<Relationship>(e).parent;
  }
}

void LightSystem::onDirectionalLightTrackerAdded(entt::registry& r, entt::entity e) {
  auto& tracker = r.get<izz::ecs::DirectionalLightTracker>(e);
  if (tracker.light == entt::null) {
    tracker.light = r.all_of<ecs::DirectionalLight>(e) ? e : r.get<Relationship>(e).parent;
  }
}

void LightSystem::initialize() {}

void LightSystem::updateLightProperties() {
  auto pointLightTrackers = m_registry.view<izz::ecs::PointLightTracker>();
  for (auto [e, tracker] : pointLightTrackers.each()) {
    auto& pointLight = m_registry.get<izz::ecs::PointLight>(tracker.light);
    auto& geometry = m_registry.get<izz::Geometry>(e);
    auto& material = m_materialSystem.getMaterialById(geometry.materialId);
    material.setVec4(tracker.color, glm::vec4(pointLight.color, 1.0));
    material.setFloat(tracker.intensity, pointLight.intensity);
    material.setFloat(tracker.radius, pointLight.radius);
  }

  auto spotLightTrackers = m_registry.view<izz::ecs::SpotLightTracker>();
  for (auto [e, tracker] : spotLightTrackers.each()) {
    auto& spotLight = m_registry.get<izz::ecs::SpotLight>(tracker.light);
    auto& geometry = m_registry.get<izz::Geometry>(e);
    auto& material = m_materialSystem.getMaterialById(geometry.materialId);
    material.setVec4(tracker.color, glm::vec4(spotLight.color, 1.0));
    material.setFloat(tracker.intensity, spotLight.intensity);
    //    material.setFloat(tracker.radius, spotLight.radius);
  }

  auto directionalLightTrackers = m_registry.view<izz::ecs::DirectionalLightTracker>();
  for (auto [e, tracker] : directionalLightTrackers.each()) {
    auto& directionalLight = m_registry.get<izz::ecs::DirectionalLight>(tracker.light);
    auto& geometry = m_registry.get<izz::Geometry>(e);
    auto& material = m_materialSystem.getMaterialById(geometry.materialId);
    material.setVec4(tracker.color, glm::vec4(directionalLight.color, 1.0));
    material.setFloat(tracker.intensity, directionalLight.intensity);
    //    material.setFloat(tracker.radius, directionalLight.radius);
  }
}