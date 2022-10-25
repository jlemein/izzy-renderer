#pragma once

#include <ecs_light.h>
#include <izzgl_material.h>
#include <izzgl_scenedependentuniform.h>
namespace izz {
namespace ufm {

struct ForwardLighting {
  static constexpr int MAX_POINT_LIGHTS = 4;
  static constexpr int MAX_DIRECTIONAL_LIGHTS = 1;
  static constexpr int MAX_SPOT_LIGHTS = 2;

  // number of lights in order: directional, ambient, point, spotlights.
  int numberOfDirectionalLights{0};
  int numberOfAmbientLights{0};
  int numberOfPointLights{0};
  int numberOfSpotLights{0};

  struct DirectionalLight {
    glm::vec4 direction;
    glm::vec4 color;
  } directionalLights[MAX_DIRECTIONAL_LIGHTS];

  struct AmbientLight {
    glm::vec4 color;
  } ambientLight;

  struct PointLight {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float radius;
    float __padding[2];
  } pointLights[MAX_POINT_LIGHTS];

  struct SpotLight {
    glm::vec3 position;
    float umbra;
    glm::vec3 direction;
    float penumbra;
    glm::vec4 color;
  } spotLights[MAX_SPOT_LIGHTS];

  static inline const char* BUFFER_NAME = "ForwardLighting";
};

class ForwardLightingUniform : public izz::gl::IUniformBuffer {
 public:
  ForwardLightingUniform(entt::registry& registry)
    : m_registry{registry} {}

  virtual ~ForwardLightingUniform() = default;

  void* allocate(size_t& t) override {
    t = sizeof(ForwardLighting);
    return new ForwardLighting;
  }

  void destroy(void* data) override {
    auto forwardLighting = reinterpret_cast<ForwardLighting*>(data);
    delete forwardLighting;
  }

  void onFrameStart(float dt, float time) override {
    auto pointLights = m_registry.view<izz::ecs::Transform, izz::ecs::PointLight>();
    auto directionalLights = m_registry.view<izz::ecs::Transform, izz::ecs::DirectionalLight>();
    auto spotLights = m_registry.view<izz::ecs::Transform, izz::ecs::SpotLight>();
    auto ambientLights = m_registry.view<izz::ecs::AmbientLight>();

    ufm::ForwardLighting* forward = &m_forwardLightingData;
    forward->ambientLight.color = glm::vec4(0.0F);
    for (auto [e, light] : ambientLights.each()) {
      forward->ambientLight.color += glm::vec4(light.intensity * light.color, 0.0F);
    }

    int i = 0;
    for (auto [e, transform, light] : spotLights.each()) {
      forward->spotLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      forward->spotLights[i].direction = transform.worldTransform * glm::vec4(0, 0, -1, 0);
      forward->spotLights[i].umbra = cos(light.umbra);
      forward->spotLights[i].penumbra = cos(light.penumbra);
      forward->spotLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_SPOT_LIGHTS) {
        break;
      }
    }
    forward->numberOfSpotLights = i;

    i = 0;
    for (auto [e, transform, light] : pointLights.each()) {
      forward->pointLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      forward->pointLights[i].intensity = light.intensity;
      forward->pointLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_POINT_LIGHTS) {
        break;
      }
    }
    forward->numberOfPointLights = i;

    i = 0;
    for (auto [e, transform, light] : directionalLights.each()) {
      forward->directionalLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      forward->directionalLights[i].direction = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_DIRECTIONAL_LIGHTS) {
        break;
      }
    }

    // because we iterated the point lights, now we exactly know the number of lights.
    forward->numberOfDirectionalLights = i;

    i = 0;
    for (auto [e, transform, light] : spotLights.each()) {
      forward->spotLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      forward->spotLights[i].direction = transform.worldTransform * glm::vec4(0, 0, -1, 0);
      forward->spotLights[i].umbra = cos(light.umbra);
      forward->spotLights[i].penumbra = cos(light.penumbra);
      forward->spotLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_SPOT_LIGHTS) {
        break;
      }
    }
    forward->numberOfSpotLights = i;
  }

  void onUpdate(izz::gl::UniformBufferPtr data, const gl::Material& material, float dt, float time) override {
    auto forward = reinterpret_cast<izz::ufm::ForwardLighting*>(data);
    std::memcpy(forward, &m_forwardLightingData, sizeof(ForwardLighting));
  }

  void onEntityUpdate(entt::entity e, izz::gl::Material& material) override {}

 private:
  entt::registry& m_registry;
  ufm::ForwardLighting m_forwardLightingData;
};

}  // namespace ufm
}  // namespace izz