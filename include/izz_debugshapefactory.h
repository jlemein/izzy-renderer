//
// Created by jlemein on 01-12-20.
//
#pragma once

#include "izz_debug.h"
#include "izzgl_materialsystem.h"

namespace izz {

namespace geo {
struct Mesh;
}

namespace gl {
class MaterialSystem;
class MeshSystem;
}

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
  DebugShapeFactory(izz::gl::MaterialSystem& materialSystem,
                    izz::gl::MeshSystem& meshSystem);

  DebugModel MakeBoundingBox(entt::registry& registry, entt::entity target);

  DebugModel MakeEulerArrow(entt::registry& registry, entt::entity target);

  DebugModel MakeModel(DebugShape shape, entt::registry& registry, entt::entity target);

  DebugModel MakeCameraBox(entt::registry& registry,
                           entt::entity target);  // SceneGraphEntity& target);

 private:
  izz::gl::MaterialSystem& m_materialSystem;
  izz::gl::MeshSystem& m_meshSystem;
};

}  // namespace izz
