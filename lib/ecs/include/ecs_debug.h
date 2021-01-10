//
// Created by jlemein on 30-11-20.
//

#ifndef ARTIFAX_ECS_DEBUG_H
#define ARTIFAX_ECS_DEBUG_H

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace artifax {
namespace ecs {
enum class DebugShape {
  kBox = 0,  /// @brief Box shape.
  kSphere,   /// @brief Spherical debug shape.
  kCamera,   /// @brief Simple camera look-a-like box representation
  kWireframe /// @brief Changes the attached mesh into wireframe mode
};

/**
 * @brief Used to visualize debug information for entities, such as the local
 * orientation, the bounding box dimensions and more.
 */
struct Debug {
  /// Attached target to show debug information for.
  entt::entity target{entt::null};

  /// Shape for the debug representation.
  DebugShape shape{DebugShape::kBox};

  /// Increases size of shape when size < 0.25.
  double minShapeSize{0.25};

  // Hides the debugged entity
  bool hideTarget{true};

  bool isEulerArrowVisible{true};

  // Renderable data
};

} // namespace ecs
} // namespace artifax

#endif // ARTIFAX_ECS_DEBUG_H
