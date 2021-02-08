//
// Created by jlemein on 08-02-21.
//

#ifndef RENDERER_GEO_CAMERA_H
#define RENDERER_GEO_CAMERA_H

#include <string>
#include <glm/glm.hpp>

namespace affx {
namespace geo {

struct Camera {
public:
  std::string name;
  glm::vec3 lookAt;
  glm::vec3 up;
  glm::vec3 position;
  float fovx, fovy;
  float aspect;
  float near, far;
};

} // end of package
} // end of namespace

#endif // RENDERER_GEO_CAMERA_H
