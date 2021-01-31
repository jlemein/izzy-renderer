//
// Created by jlemein on 29-11-20.
//

#ifndef GLVIEWER_ECSW_CAMERA_H
#define GLVIEWER_ECSW_CAMERA_H

namespace affx
{
namespace ecsw
{

#include <entt/fwd.hpp>

class Camera {
 public:
  Camera(entt::entity e) {}

  void setPosition();
  void setLookAt();

 private:
  entt::entity m_entity;

};
}
}
#endif  // GLVIEWER_ECSW_CAMERA_H
