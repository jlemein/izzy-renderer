//
// Created by jlemein on 16-11-20.
//

#ifndef GLVIEWER_ECS_CAMERA_H
#define GLVIEWER_ECS_CAMERA_H

#include <vector>
#include <entt/fwd.hpp>

namespace affx {
namespace ecs {

// offscreen render target acts as a camera, but it is not an actual camera.
// It could also be called virtual camera.
struct OrthographicView {
  float left;
  float right;
  float top;
  float bottom;
  float near;
  float far;
};

// these are buffers used for offscreen rendering.
struct ShadowMap {
  float width;
  float height;
  int pixelDepth;
};

struct PosteffectCollection {
  // post effects refer to view based materials
  std::vector<entt::entity> posteffects;
};

struct Posteffect {

};

struct Camera {
  float fovx {120.0F};
  float aspect {1.0};
  float zNear {0.1F}, zFar {1000.0F};
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECS_CAMERA_H
