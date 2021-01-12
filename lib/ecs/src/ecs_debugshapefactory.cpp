//
// Created by jlemein on 10-01-21.
//
#include <ecs_debugshapefactory.h>
#include <ecs_debugmodel.h>
#include <ecs_renderable.h>

#include <ecs_transformutil.h>
#include <entt/entt.hpp>
#include <shp_mesh.h>
#include <shp_primitivefactory.h>
#include <shp_shapeutil.h>

using namespace artifax::ecs;
using namespace artifax;

DebugModel DebugShapeFactory::MakeBoundingBox(entt::registry& registry, entt::entity target) {
  DebugModel box;

  const auto& transform = registry.get<Transform>(target);

  if( registry.has<Renderable>(target)){
    auto& r = registry.get<Renderable>(target);
    std::cout << "Name is: " << r.name << std::endl;
  }


  box.mesh.push_back(shp::PrimitiveFactory::MakeBox(1.0F, 1.0F, 1.0F));
  box.shader.push_back(Shader{.vertexShaderFile = "assets/shaders/debug.vert.spv",
      .fragmentShaderFile = "assets/shaders/debug.frag.spv"});

  box.renderable.push_back(Renderable{.isWireframe = true});
  box.transformations.push_back(Transform{});

  // by default use the
  glm::vec3 size(1.0F);
  glm::vec3 center {0.0F};

  if (registry.has<shp::Mesh>(target)) {
    // if the target entity has a mesh component,
    // then place the bounding box around the mesh
    auto targetMesh = registry.get<shp::Mesh>(target);
    auto bb = shp::ShapeUtil::computeBoundingBox(targetMesh);
    auto size = shp::BoundingBoxUtil::getSize(bb);
    auto center = shp::BoundingBoxUtil::getCenter(bb);
  } else {
    // if there is no mesh, then add a bounding box around the
    // world position with size determined by debug component.
    const auto& debug = registry.get<Debug>(target);
    size = glm::vec3(debug.minShapeSize);
    center = transform.worldTransform[3];
  }

  // scale and move the unit bounding box of the debug shape to match exactly with the bounding box of the target entity.
  TransformUtil::Scale(box.transformations[0], size);
  TransformUtil::Translate(box.transformations[0], center);

  return box;
}

DebugModel DebugShapeFactory::MakeEulerArrow(entt::registry& registry, entt::entity target) {
  DebugModel eulerArrow;

  auto xAxis = shp::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  auto yAxis = shp::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  auto zAxis = shp::PrimitiveFactory::MakeCylinder(0.05F, 2.0F);
  eulerArrow.mesh.push_back(xAxis);
  eulerArrow.mesh.push_back(yAxis);
  eulerArrow.mesh.push_back(zAxis);

  eulerArrow.renderable.push_back(Renderable{.name = "EulerX"});
  eulerArrow.renderable.push_back(Renderable{.name = "EulerY"});
  eulerArrow.renderable.push_back(Renderable{.name = "EulerZ"});

  auto shader = Shader{.vertexShaderFile = "assets/shaders/diffuse-color.vert.spv",
      .fragmentShaderFile = "assets/shaders/diffuse-color.frag.spv"};

  shader.setProperty("ColorBlock", glm::vec4(1.0F, 0.0F, 0.0F, 1.0F));
  eulerArrow.shader.push_back(shader);

  shader = Shader{.vertexShaderFile = "assets/shaders/diffuse-color.vert.spv",
      .fragmentShaderFile = "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock", glm::vec4(0.0F, 1.0F, 0.0F, 1.0F));
  eulerArrow.shader.push_back(shader);

  shader = Shader{.vertexShaderFile = "assets/shaders/diffuse-color.vert.spv",
      .fragmentShaderFile = "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock", glm::vec4(0.0F, 0.0F, 1.0F, 1.0F));
  eulerArrow.shader.push_back(shader);

  // no need to transform Y, it is already in the right orientation
  Transform transformX;
  Transform transformY;
  Transform transformZ;
  TransformUtil::RotateEuler(transformX, glm::vec3(0.0F, 0.0F, 1.0F), glm::radians(90.0F));
  TransformUtil::Translate(transformX, glm::vec3(1.0F, 0.0F, 0.0F));
  TransformUtil::Translate(transformY, glm::vec3(0.0F, 1.0F, 0.0F));
  TransformUtil::RotateEuler(transformZ, glm::vec3(1.0F, 0.0F, 0.0F), glm::radians(90.0F));
  TransformUtil::Translate(transformZ, glm::vec3(0.0F, 0.0F, 1.0F));

  eulerArrow.transformations.push_back(transformX);
  eulerArrow.transformations.push_back(transformY);
  eulerArrow.transformations.push_back(transformZ);

  return eulerArrow;
}

DebugModel DebugShapeFactory::MakeModel(DebugShape shape, entt::registry& registry, entt::entity target) {
  switch (shape) {
  case DebugShape::kBox:
    return MakeBoundingBox(registry, target);

  case DebugShape::kEulerArrow:
    return MakeEulerArrow(registry, target);

  default:
    throw std::runtime_error("Unsupported debug shape");
  }
}