#pragma once

#include <gl_renderable.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <uniform_forwardlighting.h>
#include <entt/entity/registry.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "ecs_light.h"
#include "izzgl_entityfactory.h"

namespace izz {
class Izzy;

namespace gl {

class MeshSystem;
class MaterialSystem;

class LightSystem {
 public:
  LightSystem(entt::registry& registry, MaterialSystem& materialSystem, MeshSystem& meshSystem);

  /**
   * Called by entt, when a \see PointLightTracker is added.
   * @param r The entt registry.
   * @param e The entity.
   */
  void onPointLightTrackerAdded(entt::registry& r, entt::entity e);

  /**
   * Called by entt, when a \see DirectionalLightTracker is added.
   * @param r The entt registry.
   * @param e The entity.
   */
  void onDirectionalLightTrackerAdded(entt::registry& r, entt::entity e);

  /**
   * Called by entt, when a \see SpotLightTracker is added.
   * @param r The entt registry.
   * @param e The entity.
   */
  void onSpotLightTrackerAdded(entt::registry& r, entt::entity e);

  /**
   * Initialization method called after the scene is defined, but before the simulation loop is started.
   * Validates the scene registry and assigns renderable and material components, if needed, to light sources
   * that have a mesh but no material component assigned.
   */
  void initialize();

  void setRenderSystem(izz::gl::RenderSystem& renderSystem);

//  void initLightingUbo(RenderState& r, const Material& material);
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

  MaterialId getLightMaterial() {
    return m_lightMaterial;
  }

 private:
  entt::registry& m_registry;
  izz::gl::MaterialSystem& m_materialSystem;
  izz::gl::MeshSystem& m_meshSystem;
  std::unique_ptr<izz::EntityFactory> m_entityFactory {nullptr};

  /// default material that is assigned for entities with Mesh and PointLight.
  MaterialId m_lightMaterial{MATERIAL_UNDEFINED};

  void updatePointLightVisualization(MaterialId material, const izz::ecs::PointLight& pointLight);
};

}  // namespace gl
}  // namespace izz
