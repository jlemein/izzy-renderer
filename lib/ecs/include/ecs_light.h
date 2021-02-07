//
// Created by jlemein on 11-01-21.
//

#ifndef GLVIEWER_ECS_LIGHT_H
#define GLVIEWER_ECS_LIGHT_H

namespace affx {
namespace ecs {

/**!
 * Simple lighting representation.
 * For positional or directional light sources (a transform need to be attached).
 * If no transform is attached, then the light source is ambient.
 */
struct Light {
  glm::vec3 intensity {1.0F};
  glm::vec3 color{1.0F};
  glm::vec3 ambient {0.0F};
  glm::vec3 specular {0.0F};

  /**!
   * If set to true, then the light point is assumed to be directional.
   * If false, then it is assumed to be a point light.
   */
//  bool isDirectional {false};
};

}
}
#endif // GLVIEWER_ECS_LIGHT_H
