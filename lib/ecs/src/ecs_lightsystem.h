#pragma once

#include <glm/glm.hpp>
#include <geo_material.h>
#include <ecs_renderable.h>
#include <entt/entity/registry.hpp>

namespace lsw {
namespace ecs {

struct Renderable;

struct UPointLight {
  glm::vec3 position;
  float intensity;
  glm::vec4 color;
  float attenuation;
};

struct UDirectionalLight {
  glm::vec3 direction;
  float intensity;
  glm::vec4 color;
};

struct USpotlight {
  glm::vec3 position;
  float intensity;
  glm::vec4 color;
  float attenuation;
};

struct UAmbientLight {
  float intensity;
  glm::vec4 color;
};

struct ULighting {
  // number of lights in order: directional, ambient, point, spotlights.
  glm::ivec4 numberOfLights;

  UDirectionalLight directionalLight;
  UAmbientLight ambientLight;
  UPointLight pointLights[4];
  USpotlight spotlights[2];

  static inline const char* PARAM_NAME = "Lighting2";
};

class LightSystem {
 public:
  LightSystem(entt::registry& registry);

  void initLightingUbo(Renderable& r, const geo::Material& material);
  void updateLightProperties();

 private:
  entt::registry& m_registry;
  UniformLighting m_oldModel;
  ULighting m_lighting;
};

}  // namespace ecs
}  // namespace lsw
