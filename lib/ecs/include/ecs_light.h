//
// Created by jlemein on 11-01-21.
//

#pragma once

#include <glm/glm.hpp>

namespace izz {
namespace ecs {

struct AmbientLight {
  float intensity{1.0F};
  glm::vec3 color {.15F};
  int materialId {-1};
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
