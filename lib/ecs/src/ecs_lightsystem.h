#pragma once

#include <glm/glm.hpp>
#include <geo_material.h>
#include <ecs_renderable.h>
#include <entt/entity/registry.hpp>

namespace lsw {
namespace ecs {

struct Renderable;

struct UPointLight {
  glm::vec4 position;
  glm::vec4 color;
  float intensity;
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

// manually pad the C struct, because GPU adds padding for std140
struct UAmbientLight {
  glm::vec4 color;
  float intensity;
  float __padding[3];
};

struct ForwardLighting {
  // number of lights in order: directional, ambient, point, spotlights.
  glm::ivec4 numberOfLights;

  UDirectionalLight directionalLight;
  UAmbientLight ambientLight;
  UPointLight pointLights[4];
  USpotlight spotlights[2];

  static inline const char* PARAM_NAME = "ForwardLighting";
};

class LightSystem {
 public:
  LightSystem(entt::registry& registry);

  void initLightingUbo(Renderable& r, const geo::Material& material);
  void updateLightProperties();

  int getActiveLightCount() const;

 private:
  entt::registry& m_registry;
  UniformLighting m_oldModel;
  ForwardLighting m_forwardLighting;
};

}  // namespace ecs
}  // namespace lsw
