#pragma once

#include <ecs_light.h>
#include <izzgl_material.h>
#include <izzgl_mvp.h>
#include <izzgl_scenedependentuniform.h>
#include "izzgl_material.h"
namespace izz {
namespace ufm {

struct UPointLight {
  glm::vec4 position;
  glm::vec4 color;
  float intensity;
  float linearAttenuation;
  float quadraticAttenuation;
  float __padding;
};

struct UDirectionalLight {
  glm::vec3 direction = glm::vec3(0.0F, 1.0F, 0.0F);
  float intensity = 1.0F;
  glm::vec4 color = glm::vec4(1.0);
};

// manually pad the C struct, because GPU adds padding for std140
struct UAmbientLight {
  glm::vec4 color;
  float intensity;
  float __padding[3];
};

struct ForwardLighting {
  static constexpr int MAX_POINT_LIGHTS = 3;
  static constexpr int MAX_DIRECTIONAL_LIGHTS = 1;
  static constexpr int MAX_SPOT_LIGHTS = 2;

  // number of lights in order: directional, ambient, point, spotlights.
  glm::ivec4 numberOfLights;
  //  glm::vec4 ambient = glm::vec4(0.0);

  UDirectionalLight directionalLight;
  UAmbientLight ambientLight;
  UPointLight pointLights[MAX_POINT_LIGHTS];

  struct SpotLight {
    glm::vec3 position;
    float umbra;
    glm::vec3 direction;
    float penumbra;
    glm::vec4 color;
  } spotLights[MAX_SPOT_LIGHTS];

  static inline const char* BUFFER_NAME = "ForwardLighting";
};
//
// struct ForwardLighting {
//  static constexpr int MAX_POINT_LIGHTS = 3;
//  static constexpr int MAX_DIRECTIONAL_LIGHTS = 1;
//  static constexpr int MAX_SPOT_LIGHTS = 2;
//
//  glm::vec3 viewPos;
//  int numberOfDirectionalLights {0};
//  int numberOfPointLights {0};
//  int numberOfSpotLights {0};
//  int _padding[2];
//  glm::vec4 ambient = glm::vec4(0.0);
//
//  struct DirectionalLight {
//    glm::vec4 position;
//    glm::vec4 color;
//  } directionalLights[MAX_DIRECTIONAL_LIGHTS];
//
//  struct PointLight {
//    glm::vec4 position;
//    glm::vec4 color;
//    float intensity;
//    float radius;
//
//    float _padding[2];
//  } pointLights[MAX_POINT_LIGHTS];
//
//  struct SpotLight {
//    glm::vec3 position;
//    float umbra;
//    glm::vec3 direction;
//    float penumbra;
//    glm::vec4 color;
//  } spotLights[MAX_SPOT_LIGHTS];
//
//  static inline const char* BUFFER_NAME = "ForwardLighting";
//};

// class ForwardLightingManager : public UniformBlockManager {
//  public:
//   void* CreateUniformBlock(size_t& t) override {
//     t = sizeof(ForwardLighting);
//     return new ForwardLighting;
//   }
//
//   void DestroyUniformBlock(void* data) override {
//     auto forwardLighting = reinterpret_cast<ForwardLighting*>(data);
//     delete forwardLighting;
//   }
//   void UpdateUniform(void* data, const gl::Material& m) override {
//     // intentionally left empty. There is no material data that needs to be passed in this structure.
//   }
// };

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
    //    izz::ufm::ForwardLighting* forward = nullptr;
    //    try {
    //      auto uniformBuffer = mat.uniformBuffers.at(izz::ufm::ForwardLighting::BUFFER_NAME);
    //      forward = reinterpret_cast<izz::ufm::ForwardLighting*>(uniformBuffer.data);
    //    } catch(std::out_of_range e) {
    //      auto matTemplate = m_renderSystem.getMaterialSystem().getMaterialTemplateFromMaterial(mat.id);
    //      throw std::runtime_error(fmt::format("Cannot find uniform buffer '{}' in material '{}' (template: '{}')",
    //      izz::ufm::ForwardLighting::BUFFER_NAME, mat.name, matTemplate.vertexShader));
    //    }

    // process light information from the light system
    auto pointLights = m_registry.view<izz::ecs::Transform, izz::ecs::PointLight>();
    auto directionalLights = m_registry.view<izz::ecs::Transform, izz::ecs::DirectionalLight>();
    auto spotLights = m_registry.view<izz::ecs::Transform, izz::ecs::SpotLight>();
    auto ambientLights = m_registry.view<izz::ecs::AmbientLight>();

    ufm::ForwardLighting* forward = &m_forwardLightingData;
    forward->ambientLight.color = glm::vec4(0.0F);
    forward->ambientLight.intensity = 1.0F;
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
    forward->numberOfLights[3] = i;

    i = 0;
    for (auto [e, transform, light] : pointLights.each()) {
      forward->pointLights[i].color = light.intensity * glm::vec4(light.color, 0.0F);
      //    forward->pointLights[i].linearAttenuation = light.radius;
      //    forward->pointLights[i].quadraticAttenuationAttenuation = light.radius;
      forward->pointLights[i].intensity = light.intensity;
      forward->pointLights[i].position = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_POINT_LIGHTS) {
        break;
      }
    }
    forward->numberOfLights[2] = i;

    i = 0;
    for (auto [e, transform, light] : directionalLights.each()) {
      forward->directionalLight.color = light.intensity * glm::vec4(light.color, 0.0F);
      forward->directionalLight.direction = transform.worldTransform[3];

      // check if the count is exceeding the max point lights (needed because entt only gives us a size_hint).
      if (++i >= izz::ufm::ForwardLighting::MAX_DIRECTIONAL_LIGHTS) {
        break;
      }
    }

    // because we iterated the point lights, now we exactly know the number of lights.
    forward->numberOfLights[0] = i;
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