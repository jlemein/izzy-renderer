//
// Created by jlemein on 30-11-20.
//

#ifndef ARTIFAX_ECS_DEBUG_H
#define ARTIFAX_ECS_DEBUG_H

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace lsw {
namespace ecs {
enum class DebugShape {
  kUndefined = -1, /// @brief Undefined, let the debug system choose appropriate.
  kBox = 0,  /// @brief Box shape.
  kEulerArrow, /// @brief Euler arrow visualization
  kSphere,   /// @brief Spherical debug shape.
  kCamera,   /// @brief Simple camera look-a-like box representation
};

/**
 * @brief Used to visualize debug information for entities, such as the local
 * orientation, the bounding box dimensions and more.
 */
struct Debug {
  /// Shape for the debug representation.
  DebugShape shape{DebugShape::kBox};

//  std::function<DebugShape()> shapeFn;

  /// Increases size of shape when size < 0.25.
  double minShapeSize{0.10};

  // Hides the debugged entity
  bool hideTarget{true};

  bool isEulerArrowVisible{true};

  // Renderable data
};

} // namespace ecs
} // namespace lsw

#endif // ARTIFAX_ECS_DEBUG_H
