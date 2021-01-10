//
// Created by jlemein on 01-12-20.
//
#include <ecs_debug.h>
#include <ecs_debugsystem.h>
#include <ecs_relationship.h>
#include <ecs_renderable.h>
#include <ecs_transform.h>
#include <shp_curve.h>
#include <shp_mesh.h>
#include <shp_primitivefactory.h>
#include <shp_shapeutil.h>

#include <ecs_relationshiputil.h>
#include <ecs_transformutil.h>
#include <entt/entt.hpp>

using namespace artifax;
using namespace artifax::ecs;

DebugSystem::DebugSystem(entt::registry &registry) : m_registry{registry} {
  DebugPrefab box;
  box.mesh = shp::PrimitiveFactory::MakeBox(1.0F, 1.0F, 1.0F);
  box.shader = Shader{.vertexShaderFile = "assets/shaders/debug.vert.spv",
                      .fragmentShaderFile = "assets/shaders/debug.frag.spv"};

  box.renderable = Renderable{.isWireframe = true};

  m_debugPrefabs[DebugShape::kBox] = box;
}

void DebugSystem::init() {
  // the debug system works as follows.
  // whenever an entity has a debug component attached
  // * it either transforms the attached renderable mesh into a wireframe model
  // * it attaches debug visualization, by creating a new renderable entity as a
  // child of the entity
  auto view = m_registry.view<Debug>();
  for (auto e : view) {
    Debug debug = view.get<Debug>(e);
    //    if (targetMesh_p == nullptr) {
    //      std::cerr << "Something went wrong" << std::endl;
    //    }

    Renderable *renderable_p = m_registry.try_get<Renderable>(e);
    if (renderable_p) {
      std::cout << "*Name of renderable is " << renderable_p->name << std::endl;
    }

    // TODO: as part of init, make a factory method makeDebug that instantiates
    // a new entity based on assign components
    const DebugPrefab &prefab = m_debugPrefabs.at(debug.shape);

    auto debugEntity = m_registry.create();
    auto &r = m_registry.emplace<Renderable>(debugEntity, prefab.renderable);
    r.name = "Debugg";

    auto &debugMesh = m_registry.emplace<shp::Mesh>(debugEntity, prefab.mesh);
    m_registry.emplace<Shader>(debugEntity, prefab.shader);

    auto &debugTransform = m_registry.emplace<Transform>(debugEntity);

    auto &targetMesh = m_registry.get<shp::Mesh>(e);
    auto bb = shp::ShapeUtil::computeBoundingBox(targetMesh);

    auto size = shp::BoundingBoxUtil::getSize(bb);
    auto center = shp::BoundingBoxUtil::getCenter(bb);

    // scale and move the unit bounding box of the debug shape to match exactly
    // with the bounding box of the target entity.
    TransformUtil::Scale(debugTransform, size);
    TransformUtil::Translate(debugTransform, center);

    // transform the debug bounding box with t
    RelationshipUtil::MakeChild(m_registry, e, debugEntity);
  }
}
