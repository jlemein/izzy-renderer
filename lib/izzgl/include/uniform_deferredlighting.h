#pragma once

#include <izzgl_scenedependentuniform.h>
#include "ecs_camera.h"
#include "izzgl_material.h"

namespace izz {
namespace ufm {

/**
 * CPU based parameter mapping of the uniform data.
 * This buffer maps with the GLSL shader structure, hence the padding to align these structs.
 */
struct DeferredLighting {
  static constexpr int MAX_DIRECTIONAL_LIGHTS = 2;
  static constexpr int MAX_POINT_LIGHTS = 32;
  static constexpr int MAX_SPOT_LIGHTS = 4;

  glm::vec3 viewPos;
  int numberOfDirectionalLights{0};
  int numberOfPointLights{0};
  int numberOfSpotLights{0};
  int _padding[2];
  glm::vec4 ambient = glm::vec4(0.0);

  struct DirectionalLight {
    glm::vec4 position;
    glm::vec4 color;
  } directionalLights[MAX_DIRECTIONAL_LIGHTS];

  struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float radius;

    float _padding[2];
  } pointLights[MAX_POINT_LIGHTS];

  struct SpotLight {
    glm::vec3 position;
    float umbra;
    glm::vec3 direction;
    float penumbra;
    glm::vec4 color;
  } spotLights[MAX_SPOT_LIGHTS];

  static inline const char* BUFFER_NAME = "DeferredLighting";
};

class DeferredLightingUniform : public izz::gl::IUniformBuffer {
 public:
  DeferredLightingUniform(entt::registry& registry)
    : m_registry{registry} {}

  virtual ~DeferredLightingUniform() = default;

  void* allocate(size_t& t) override {
    t = sizeof(DeferredLighting);
    return new DeferredLighting;
  }

  void destroy(void* data) override {
    auto deferredLighting = reinterpret_cast<DeferredLighting*>(data);
    delete deferredLighting;
  }

  void onFrameStart(float ft, float time) override {
    auto directionalLights = m_registry.view<izz::ecs::Transform, izz::ecs::DirectionalLight>();
    auto pointLights = m_registry.view<izz::ecs::Transform, izz::ecs::PointLight>();
    auto spotLights = m_registry.view<izz::ecs::Transform, izz::ecs::SpotLight>();
    auto ambientLights = m_registry.view<izz::ecs::AmbientLight>();

    ufm::DeferredLighting* deferred = &m_deferredLightingData;
    auto cameraEntity = m_registry.view<izz::ecs::Camera>()[0];
    auto& cameraTransform = m_registry.get<izz::ecs::Transform>(cameraEntity);
    deferred->viewPos = glm::inverse(cameraTransform.worldTransform)[3];

    int i = 0;
    for (auto [e, transform, light] : directionalLights.each()) {
      deferred->directionalLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      deferred->directionalLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::DeferredLighting::MAX_DIRECTIONAL_LIGHTS) {
        break;
      }
    }
    deferred->numberOfDirectionalLights = i;

    i = 0;
    for (auto [e, transform, light] : pointLights.each()) {
      deferred->pointLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      deferred->pointLights[i].radius = light.radius;
      deferred->pointLights[i].intensity = light.intensity;
      deferred->pointLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::DeferredLighting::MAX_POINT_LIGHTS) {
        break;
      }
    }
    deferred->numberOfPointLights = i;

    i = 0;
    for (auto [e, transform, light] : spotLights.each()) {
      deferred->spotLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      deferred->spotLights[i].direction = transform.worldTransform * glm::vec4(0, 0, -1, 0);
      deferred->spotLights[i].umbra = cos(light.umbra);
      deferred->spotLights[i].penumbra = cos(light.penumbra);
      deferred->spotLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::DeferredLighting::MAX_SPOT_LIGHTS) {
        break;
      }
    }
    deferred->numberOfSpotLights = i;

    deferred->ambient = glm::vec4(0.0F);
    for (auto [e, light] : ambientLights.each()) {
      deferred->ambient += glm::vec4(light.intensity * light.color, 0.0F);
    }
  }

  void onUpdate(izz::gl::UniformBufferPtr data, const gl::Material& material, float dt, float time) override {
    DeferredLighting* deferred = reinterpret_cast<izz::ufm::DeferredLighting*>(data);
    std::memcpy(deferred, &m_deferredLightingData, sizeof(DeferredLighting));
  }

  void onEntityUpdate(entt::entity e, izz::gl::Material& material) override {}

 private:
  entt::registry& m_registry;
  ufm::DeferredLighting m_deferredLightingData;
};

}  // namespace ufm
}  // namespace izz