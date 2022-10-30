//
// Created by jlemein on 21-09-22.
//

#pragma once

#include <math.h>
#include "ecs_camera.h"
#include <ecs_transformutil.h>

namespace izz {
struct Util {
  static inline double RadiansToDegrees(double radians) {
    return 180.0 * radians / M_PI;
  }
  static inline float RadiansToDegrees(float radians) {
    return 180.0F * radians / static_cast<float>(M_PI);
  }

  static inline double DegreesToRadians(double degrees) {
    return M_PI * degrees / 180.0;
  };
  static inline float DegreesToRadians(float degrees) {
    return static_cast<float>(M_PI) * degrees / 180.0F;
  };

  /**
   * Aligns the camera to view the entire scene contents.
   * @param content
   * @param camera
   */
  static inline void AlignCameraToBoundingBox(SceneGraphEntity content, SceneGraphEntity camera) {
    // update the transform chain
    ecs::TransformUtil::UpdateTransformChain(content);

    BoundingBox bb = content.has<Geometry>() ? content.get<Geometry>().aabb : BoundingBox{};

    std::vector<SceneGraphEntity> children = content.getChildren();
    for (int i = 0; i < children.size(); ++i) {
      auto& child = children[i];

      if (child.has<Geometry>()) {
        auto childBb = child.get<Geometry>().aabb;

        BoundingBox transformedAabb;
        transformedAabb.min = glm::vec3(child.get<ecs::Transform>().worldTransform * glm::vec4(childBb.min, 1.0F));
        transformedAabb.max = glm::vec3(child.get<ecs::Transform>().worldTransform * glm::vec4(childBb.max, 1.0F));
        std::cout << "Child has bounding box: " << transformedAabb.min.x << ", " << transformedAabb.min.y << ", " << transformedAabb.min.z << " -- "
                  << transformedAabb.max.x << ", " << transformedAabb.max.y << ", " << transformedAabb.max.z << std::endl;

        bb += transformedAabb;
      }

      auto moreChildren = child.getChildren();
      children.insert(children.end(), moreChildren.begin(), moreChildren.end());
    }

    auto c = camera.get<izz::ecs::Camera>();
    // now we do have bounding box of all elements.
    auto extent = bb.getSize();
    float fovy = c.fovx / c.aspect;
    auto minDistanceZ = std::max(extent.x * 0.5 / tan(DegreesToRadians(0.5*c.fovx)), extent.y * 0.5 / tan(DegreesToRadians(0.5*fovy)));

    auto& transform = camera.get<ecs::Transform>();
    auto eye = bb.getCenter() += minDistanceZ * 1.2;
    transform.localTransform = glm::inverse(glm::lookAt(eye, bb.getCenter(), glm::vec3(0.0, 1.0, 0.0)));
  }
};

}  // namespace izz
