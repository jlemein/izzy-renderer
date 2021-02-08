//
// Created by jlemein on 16-11-20.
//

#ifndef GLVIEWER_ECS_CAMERA_H
#define GLVIEWER_ECS_CAMERA_H

namespace affx {
namespace ecs {

struct Camera {
  float fovx {120.0F};
  float aspect {1.0};
  float zNear {0.1F}, zFar {1000.0F};
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECS_CAMERA_H
