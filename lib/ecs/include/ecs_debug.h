//
// Created by jlemein on 30-11-20.
//

#ifndef GLVIEWER_ECS_DEBUG_H
#define GLVIEWER_ECS_DEBUG_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

namespace artifax
{
namespace ecs
{
enum class DebugShape {
  kBox = 0,  /// @brief Box shape.
  kSphere,   /// @brief Spherical debug shape.
  kCamera    /// @brief Simple camera look-a-like box representation
};

/**
 * @brief Used to visualize debug information for entities, such as the local orientation,
 * the bounding box dimensions and more.
 */
struct Debug {
  entt::entity target{entt::null};      /// Attached target to show debug information for.
  DebugShape shape{DebugShape::kBox};   /// Shape for the debug representation.
  double minShapeSize{0.25};            /// Increases size of shape when size < 0.25.

  bool isShapeVisible{true};
  bool isEulerArrowVisible{true};

  // Renderable data


};

}  // namespace ecs
}  // namespace artifax

#endif  // GLVIEWER_ECS_DEBUG_H
