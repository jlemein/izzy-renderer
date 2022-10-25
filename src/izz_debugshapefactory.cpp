//
// Created by jlemein on 10-01-21.
//
#include "izz_debugshapefactory.h"
#include "ecs_transformutil.h"
#include "izz_debugmodel.h"

#include <entt/entt.hpp>
#include "geo_mesh.h"
#include "geo_meshtransform.h"
#include "geo_primitivefactory.h"
#include "geo_shapeutil.h"

using namespace izz;

DebugShapeFactory::DebugShapeFactory(izz::gl::MaterialSystem& materialSystem, izz::gl::MeshSystem& meshSystem)
  : m_materialSystem{materialSystem}
  , m_meshSystem{meshSystem} {}

DebugModel DebugShapeFactory::MakeBoundingBox(entt::registry& registry, entt::entity target) {
  DebugModel box;
  box.isWireframe = true;

  const auto& transform = registry.get<ecs::Transform>(target);

  izz::Geometry r;
  auto mesh = geo::PrimitiveFactory::MakeBox("Debug_BoundingBox", 1.0F, 1.0F, 1.0F);
  auto& vertexBuffer = m_meshSystem.createVertexBuffer(mesh);
  auto& material = m_materialSystem.createMaterial("Debug");
  r.vertexBufferId = vertexBuffer.id;
  r.materialId = material.id;

  box.renderable.push_back(r);
  box.transformations.push_back(ecs::Transform{});

  // by default use the
  glm::vec3 size(1.0F);
  glm::vec3 center{0.0F};

  if (registry.all_of<geo::Mesh>(target)) {
    // if the target entity has a mesh component,
    // then place the bounding box around the mesh
    auto targetMesh = registry.get<geo::Mesh>(target);
    auto bb = geo::ShapeUtil::computeBoundingBox(targetMesh);
    auto size = geo::BoundingBoxUtil::getSize(bb);
    auto center = geo::BoundingBoxUtil::getCenter(bb);
  } else {
    // if there is no mesh, then add a bounding box around the
    // world position with size determined by debug component.
    const auto& debug = registry.get<Debug>(target);
    size = glm::vec3(debug.minShapeSize);
    center = transform.worldTransform[3];
  }

  // scale and move the unit bounding box of the debug shape to match exactly
  // with the bounding box of the target entity.
  ecs::TransformUtil::Scale(box.transformations[0], size);
  ecs::TransformUtil::Translate(box.transformations[0], center);

  return box;
}

DebugModel DebugShapeFactory::MakeEulerArrow(entt::registry& registry, entt::entity target) {
  DebugModel eulerArrow;

  eulerArrow.renderable.push_back(izz::Geometry{});
  eulerArrow.renderable.push_back(izz::Geometry{});
  eulerArrow.renderable.push_back(izz::Geometry{});

  eulerArrow.names.push_back({"EulerX"});
  eulerArrow.names.push_back({"EulerY"});
  eulerArrow.names.push_back({"EulerZ"});

  auto xAxis = geo::PrimitiveFactory::MakeCylinder("Debug_CylinderX", 0.05F, 2.0F);
  eulerArrow.renderable[0].materialId = m_materialSystem.createMaterial("Diffuse").id;
  eulerArrow.renderable[0].vertexBufferId = m_meshSystem.createVertexBuffer(xAxis).id;

  auto yAxis = geo::PrimitiveFactory::MakeCylinder("Debug_CylinderY", 0.05F, 2.0F);
  eulerArrow.renderable[1].materialId = m_materialSystem.createMaterial("Diffuse").id;
  eulerArrow.renderable[1].vertexBufferId = m_meshSystem.createVertexBuffer(yAxis).id;

  auto zAxis = geo::PrimitiveFactory::MakeCylinder("Debug_CylinderZ", 0.05F, 2.0F);
  eulerArrow.renderable[2].materialId = m_materialSystem.createMaterial("Diffuse").id;
  eulerArrow.renderable[2].vertexBufferId = m_meshSystem.createVertexBuffer(zAxis).id;

  // no need to transform Y, it is already in the right orientation
  ecs::Transform transformX;
  ecs::Transform transformY;
  ecs::Transform transformZ;
  ecs::TransformUtil::RotateEuler(transformX, glm::vec3(0.0F, 0.0F, 1.0F), glm::radians(90.0F));
  ecs::TransformUtil::Translate(transformX, glm::vec3(1.0F, 0.0F, 0.0F));
  ecs::TransformUtil::Translate(transformY, glm::vec3(0.0F, 1.0F, 0.0F));
  ecs::TransformUtil::RotateEuler(transformZ, glm::vec3(1.0F, 0.0F, 0.0F), glm::radians(90.0F));
  ecs::TransformUtil::Translate(transformZ, glm::vec3(0.0F, 0.0F, 1.0F));

  eulerArrow.transformations.push_back(transformX);
  eulerArrow.transformations.push_back(transformY);
  eulerArrow.transformations.push_back(transformZ);

  return eulerArrow;
}

DebugModel DebugShapeFactory::MakeCameraBox(entt::registry& registry, entt::entity target) {
  DebugModel cameraBox;

  float cameraFrontHeight = 0.4F;
  float cameraBoxDepth = 1.0F;
  float cameraBoxWidth = 0.5F;

  izz::Geometry r;
  auto cameraBoxMesh = geo::PrimitiveFactory::MakeBox("Debug_CameraBox", cameraBoxWidth, cameraBoxWidth, cameraBoxDepth);
  geo::MeshTransform::Translate(cameraBoxMesh, glm::vec3(0.0F, 0.0F, 0.5F * cameraBoxDepth + cameraFrontHeight));
  r.vertexBufferId = m_meshSystem.createVertexBuffer(cameraBoxMesh).id;
  r.materialId = m_materialSystem.createMaterial("Diffuse").id;
  cameraBox.renderable.push_back(r);
  cameraBox.transformations.push_back(ecs::Transform{});

  // front of camera
  izz::Geometry cameraFrontRenderable;
  auto cameraFront = geo::PrimitiveFactory::MakePyramid("Debug_Pyramid", cameraBoxWidth * 0.8F, cameraFrontHeight);
  geo::MeshTransform::RotateX(cameraFront, 90.0F);
  float translate = cameraFrontHeight * 0.5F;
  geo::MeshTransform::Translate(cameraFront, glm::vec3(0.0F, 0.0F, translate));

  cameraFrontRenderable.vertexBufferId = m_meshSystem.createVertexBuffer(cameraFront).id;
  cameraFrontRenderable.materialId = m_materialSystem.createMaterial("Diffuse").id;
//  shader.setProperty<izz::gl::ColorBlock>(izz::gl::ColorBlock{.color = glm::vec4(1.0F, 0.5F, 0.0F, 1.0F), .diffuseShading = true});
  cameraBox.renderable.push_back(cameraFrontRenderable);

  cameraBox.transformations.push_back(ecs::Transform{});

  return cameraBox;
}

DebugModel DebugShapeFactory::MakeModel(DebugShape shape, entt::registry& registry, entt::entity target) {
  switch (shape) {
    case DebugShape::kBox:
      return MakeBoundingBox(registry, target);

    case DebugShape::kEulerArrow:
      return MakeEulerArrow(registry, target);

    case DebugShape::kCamera:
      return MakeCameraBox(registry, target);

    default:
      throw std::runtime_error("Unsupported debug shape");
  }
}