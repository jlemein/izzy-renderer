//
// Created by jlemein on 11-01-21.
//

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <entt/entt.hpp>

namespace izz {
namespace ecs {

struct AmbientLight {
  float intensity{1.0F};
  glm::vec3 color {.15F};
  int materialId {-1};
};

/**
 * @brief Tracks point light parameters. It will update the assigned material on the current entity.
 * Materials are assigned using the Geometry component.
 *
 * @details By default, the light system will find the point light on the current entity.
 * If it's not found on the current entity, it will try to find it on the parent entity.
 * If it's also not there, then you must make sture to set the light entity yourself.
 */
struct PointLightTracker {
  entt::entity light = entt::null;

  /// @brief name of the color parameter, by default 'color'.
  std::string color = "color";
  std::string intensity = "intensity";
  std::string radius = "radius";
};

/**
 * @brief Tracks directional light parameters. It will update the assigned material on the current entity.
 * Materials are assigned using the Geometry component.
 *
 * @details By default, the light system will find the directional light on the current entity.
 * If it's not found on the current entity, it will try to find it on the parent entity.
 * If it's also not there, then you must make sure to set the light entity yourself.
 */
struct DirectionalLightTracker {
  entt::entity light = entt::null;

  /// @brief name of the color parameter, by default 'color'.
  std::string color = "color";
  std::string intensity = "intensity";
  std::string radius = "radius";
};

/**
 * @brief Tracks spot light parameters. It will update the assigned material on the current entity.
 * Materials are assigned using the Geometry component.
 *
 * @details By default, the light system will find the spot light on the current entity.
 * If it's not found on the current entity, it will try to find it on the parent entity.
 * If it's also not there, then you must make sure to set the light entity yourself.
 */
struct SpotLightTracker {
  entt::entity light = entt::null;

  /// @brief name of the color parameter, by default 'color'.
  std::string color = "color";
  std::string intensity = "intensity";
  std::string radius = "radius";
};

struct PointLight {
  float intensity{100.0F};
  float linearAttenuation{0.2F};
  float quadraticAttenuation{0.2F};
  float radius{0.01F};
  glm::vec3 color {1.F};
  int materialId {-1};
};

struct DirectionalLight {
  float intensity{1.0F};
  glm::vec3 color {1.0F, 1.0, 1.0F};
};

struct SpotLight {
  float intensity{1.F};
  glm::vec3 color {1.F};
  float umbra = M_PI_4; // 45 degrees
  float penumbra = M_PI/6.0F; // 30 degrees
};

}  // namespace ecs
}  // namespace lsw
