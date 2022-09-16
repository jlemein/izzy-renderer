#pragma once

#include <gl_renderable.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <entt/entity/registry.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "ecs_light.h"

namespace izz {
namespace gl {

struct UPointLight {
  glm::vec4 position;
  glm::vec4 color;
  float intensity;
  float linearAttenuation;
  float quadraticAttenuation;
  float __padding;
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
  LightSystem(entt::registry& registry, std::shared_ptr<MaterialSystem> materialSystem);

  /**
   * Initialization method called after the scene is defined, but before the simulation loop is started.
   * Validates the scene registry and assigns renderable and material components, if needed, to light sources
   * that have a mesh but no material component assigned.
   */
  void initialize();

  void initLightingUbo(RenderState& r, const Material& material);
  void updateLightProperties();

  /**
   * Sets the default material to use for point light sources that have meshes assigned to them.
   * This material gets assigned to entities that define a Mesh and a PointLight component, but do
   * not set a Material component.
   * @details The shader must take a ConstantLight Uniform Block as parameters to the light source. This can
   * not be customized as of yet.
   * @param material The default material to assign to point light sources if the user does not specify one.
   */
  void setDefaultPointLightMaterial(int materialId);

  /**
   * @returns the number of lights active lights in the scene.
   */
  int getActiveLightCount() const;

 private:
  entt::registry& m_registry;
  ForwardLighting m_forwardLighting;

  std::shared_ptr<MaterialSystem> m_materialSystem;

  /// default material that is assigned for entities with Mesh and PointLight.
//  std::shared_ptr<izz::gl::Material> m_lightMaterial{nullptr};
  int m_lightMaterial {-1};
};

}  // namespace gl
}  // namespace izz