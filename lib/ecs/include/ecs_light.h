//
// Created by jlemein on 11-01-21.
//

#ifndef GLVIEWER_ECS_LIGHT_H
#define GLVIEWER_ECS_LIGHT_H

#include <glm/glm.hpp>

namespace lsw {
namespace ecs {

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
  float intensity{1.0F};
  glm::vec3 color {1.0F, 1.0, 1.0F};
};

struct SpotLight {};

}  // namespace ecs
}  // namespace lsw
#endif  // GLVIEWER_ECS_LIGHT_H
