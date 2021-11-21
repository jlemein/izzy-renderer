//
// Created by jlemein on 08-02-21.
//
#pragma once

#include <string>
#include <glm/glm.hpp>

namespace lsw {
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
