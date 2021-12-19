//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_ECS_DEBUGSHAPEFACTORY_H
#define ARTIFAX_ECS_DEBUGSHAPEFACTORY_H

#include "ecs_debug.h"

namespace lsw {

namespace geo {
struct Mesh;
}

namespace ecs {

struct DebugModel;
struct SceneGraphEntity;

/**!
 * Debug shape factory creates, different to @see PrimitiveFactory, specific
 * shapes for use in debug visualization. Examples are bounding boxes, axis
 * visualization, gizmo visualizations, etcetera. These shapes are built on top
 * of the @see PrimitiveShapeFactory functionality.
 */
 // TODO consider a DebugResources class instead of this shape factory
//   resources can facilitate sharing of instances
class DebugShapeFactory {
public:
  static DebugModel MakeBoundingBox(entt::registry& registry, entt::entity target);

  static DebugModel MakeEulerArrow(entt::registry& registry, entt::entity target);

  static DebugModel MakeModel(DebugShape shape, entt::registry& registry, entt::entity target);

  static DebugModel MakeCameraBox(entt::registry &registry,
                                  entt::entity target);//SceneGraphEntity& target);
};

}
}
#endif // ARTIFAX_ECS_DEBUGSHAPEFACTORY_H
