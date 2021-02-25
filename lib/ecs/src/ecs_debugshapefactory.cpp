//
// Created by jlemein on 10-01-21.
//
#include <ecs_debugmodel.h>
#include <ecs_debugshapefactory.h>
#include <ecs_renderable.h>
#include <ecs_transformutil.h>

#include <entt/entt.hpp>
#include <geo_mesh.h>
#include <geo_meshtransform.h>
#include <geo_primitivefactory.h>
#include <geo_shapeutil.h>

using namespace affx::ecs;
using namespace affx;

DebugModel DebugShapeFactory::MakeBoundingBox(entt::registry &registry,
                                              entt::entity target) {
  DebugModel box;

  const auto &transform = registry.get<Transform>(target);

  box.mesh.push_back(geo::PrimitiveFactory::MakeBox(1.0F, 1.0F, 1.0F));
  box.material.push_back(
      geo::Material{.vertexShader = "assets/shaders/debug.vert.spv",
                    .fragmentShader = "assets/shaders/debug.frag.spv"});

  box.renderable.push_back(Renderable{.isWireframe = true});
  box.transformations.push_back(Transform{});

  // by default use the
  glm::vec3 size(1.0F);
  glm::vec3 center{0.0F};

  if (registry.has<geo::Mesh>(target)) {
    // if the target entity has a mesh component,
    // then place the bounding box around the mesh
    auto targetMesh = registry.get<geo::Mesh>(target);
    auto bb = geo::ShapeUtil::computeBoundingBox(targetMesh);
    auto size = geo::BoundingBoxUtil::getSize(bb);
    auto center = geo::BoundingBoxUtil::getCenter(bb);
  } else {
    // if there is no mesh, then add a bounding box around the
    // world position with size determined by debug component.
    const auto &debug = registry.get<Debug>(target);
    size = glm::vec3(debug.minShapeSize);
    center = transform.worldTransform[3];
  }

  // scale and move the unit bounding box of the debug shape to match exactly
  // with the bounding box of the target entity.
  TransformUtil::Scale(box.transformations[0], size);
  TransformUtil::Translate(box.transformations[0], center);

  return box;
}

DebugModel DebugShapeFactory::MakeEulerArrow(entt::registry &registry,
                                             entt::entity target) {
  DebugModel eulerArrow;

  auto xAxis = geo::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  auto yAxis = geo::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  auto zAxis = geo::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  eulerArrow.mesh.push_back(xAxis);
  eulerArrow.mesh.push_back(yAxis);
  eulerArrow.mesh.push_back(zAxis);

  eulerArrow.renderable.push_back(Renderable{});
  eulerArrow.renderable.push_back(Renderable{});
  eulerArrow.renderable.push_back(Renderable{});

  eulerArrow.names.push_back({"EulerX"});
  eulerArrow.names.push_back({"EulerY"});
  eulerArrow.names.push_back({"EulerZ"});

  auto shader =
      geo::Material{.vertexShader = "assets/shaders/diffuse-color.vert.spv",
                    .fragmentShader = "assets/shaders/diffuse-color.frag.spv"};

  shader.setProperty("ColorBlock", glm::vec4(1.0F, 0.0F, 0.0F, 1.0F));
  eulerArrow.material.push_back(shader);

  shader =
      geo::Material{.vertexShader = "assets/shaders/diffuse-color.vert.spv",
                    .fragmentShader = "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock", glm::vec4(0.0F, 1.0F, 0.0F, 1.0F));
  eulerArrow.material.push_back(shader);

  shader =
      geo::Material{.vertexShader = "assets/shaders/diffuse-color.vert.spv",
                    .fragmentShader = "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock", glm::vec4(0.0F, 0.0F, 1.0F, 1.0F));
  eulerArrow.material.push_back(shader);

  // no need to transform Y, it is already in the right orientation
  Transform transformX;
  Transform transformY;
  Transform transformZ;
  TransformUtil::RotateEuler(transformX, glm::vec3(0.0F, 0.0F, 1.0F),
                             glm::radians(90.0F));
  TransformUtil::Translate(transformX, glm::vec3(1.0F, 0.0F, 0.0F));
  TransformUtil::Translate(transformY, glm::vec3(0.0F, 1.0F, 0.0F));
  TransformUtil::RotateEuler(transformZ, glm::vec3(1.0F, 0.0F, 0.0F),
                             glm::radians(90.0F));
  TransformUtil::Translate(transformZ, glm::vec3(0.0F, 0.0F, 1.0F));

  eulerArrow.transformations.push_back(transformX);
  eulerArrow.transformations.push_back(transformY);
  eulerArrow.transformations.push_back(transformZ);

  return eulerArrow;
}

DebugModel DebugShapeFactory::MakeCameraBox(entt::registry &registry,
                                            entt::entity target) {
  DebugModel cameraBox;

  float cameraFrontHeight = 0.4F;
  float cameraBoxDepth = 1.0F;
  float cameraBoxWidth = 0.5F;

  auto cameraBoxMesh = geo::PrimitiveFactory::MakeBox(
      cameraBoxWidth, cameraBoxWidth, cameraBoxDepth);
  geo::MeshTransform::Translate(
      cameraBoxMesh,
      glm::vec3(0.0F, 0.0F, 0.5F * cameraBoxDepth + cameraFrontHeight));
  cameraBox.mesh.push_back(cameraBoxMesh);
  cameraBox.material.push_back(
      geo::Material{.vertexShader = "assets/shaders/debug.vert.spv",
                    .fragmentShader = "assets/shaders/debug.frag.spv"});
  cameraBox.renderable.push_back(Renderable{.isWireframe = false});
  cameraBox.transformations.push_back(Transform{});

  auto cameraFront = geo::PrimitiveFactory::MakePyramid(cameraBoxWidth * 0.8F,
                                                        cameraFrontHeight);
  geo::MeshTransform::RotateX(cameraFront, 90.0F);

  float translate = cameraFrontHeight * 0.5F;
  geo::MeshTransform::Translate(cameraFront, glm::vec3(0.0F, 0.0F, translate));

  cameraBox.mesh.emplace_back(std::move(cameraFront));
  auto shader =
      geo::Material{.vertexShader = "assets/shaders/debug.vert.spv",
                    .fragmentShader = "assets/shaders/debug.frag.spv"};
  shader.setProperty<ColorBlock>(ColorBlock{
      .color = glm::vec4(1.0F, 0.5F, 0.0F, 1.0F), .diffuseShading = true});
  cameraBox.material.push_back(std::move(shader));

  cameraBox.renderable.push_back(Renderable{.isWireframe = false});
  cameraBox.transformations.push_back(Transform{});

  return cameraBox;
}

DebugModel DebugShapeFactory::MakeModel(DebugShape shape,
                                        entt::registry &registry,
                                        entt::entity target) {
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