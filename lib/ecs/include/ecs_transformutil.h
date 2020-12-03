//
// Created by jlemein on 12-11-20.
//

#ifndef GLVIEWER_ECS_TRANSFORMUTIL_H
#define GLVIEWER_ECS_TRANSFORMUTIL_H

#include <glm/glm.hpp>

namespace artifax {
namespace ecs {

class Transform;

struct TransformUtil {
//  static void setWorldPosition(Transform& t, glm::vec3& e);
  static void SetPosition(Transform& t, const glm::vec3& e);
};

} // end package namespace
} // end enterprise name

#endif // GLVIEWER_ECS_TRANSFORMUTIL_H
