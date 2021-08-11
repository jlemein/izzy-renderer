//
// Created by jlemein on 11-01-21.
//

#ifndef GLVIEWER_ECS_LIGHT_H
#define GLVIEWER_ECS_LIGHT_H

#include <glm/glm.hpp>

namespace lsw {
namespace ecs {

///**!
// * Simple lighting representation.
// * For positional or directional light sources (a transform need to be attached).
// * If no transform is attached, then the light source is ambient.
// */
//struct Light {
//  float intensity{1000.0F};  // by default 1000 W
//  float attenuationQuadratic{0.2F};
//  glm::vec3 diffuseColor{1.0F};
//  glm::vec3 specularColor{0.0F};
//  glm::vec3 ambientColor{0.0F};
//};

struct AmbientLight {
  float intensity{1000.0F};
  glm::vec3 color {1.0F};
};

struct PointLight {
  float intensity{1000.0F};
  float attenuation{0.2F};
  float radius{1.0F};
  glm::vec3 color {1.F};
};

struct DirectionalLight {
  float intensity{1000.0F};
  glm::vec3 color {1.0F};
};

struct SpotLight {};

}  // namespace ecs
}  // namespace lsw
#endif  // GLVIEWER_ECS_LIGHT_H
