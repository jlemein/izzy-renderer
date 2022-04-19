//
// Created by jlemein on 01-12-20.
//
#include "ecs_debug.h"
#include "ecs_debugsystem.h"
#include "ecs_relationship.h"
#include "../include/gl_renderable.h"
#include "ecs_transform.h"
#include "geo_curve.h"
#include "geo_mesh.h"
#include "geo_primitivefactory.h"
#include "geo_shapeutil.h"

// TODO: does not belong in this package
#include "ecs_debugshapefactory.h"

#include "ecs_name.h"
#include "ecs_relationshiputil.h"
#include "ecs_transformutil.h"
#include <entt/entt.hpp>

using namespace lsw;
using namespace lsw::ecs;

DebugSystem::DebugSystem(entt::registry &registry) : m_registry{registry} {}

void DebugSystem::init() {
  auto view = m_registry.view<Debug>();
  for (auto e : view) {
    Debug debug = view.get<Debug>(e);

    izz::gl::Renderable *renderable_p = m_registry.try_get<izz::gl::Renderable>(e);
    std::string name = "Unnamed";

    if(m_registry.all_of<Name>(e)) {
      std::cout << "Adding debugger for " << m_registry.get<Name>(e).name << std::endl;
      name = m_registry.get<Name>(e).name;
    }

    DebugModel model =
        DebugShapeFactory::MakeModel(debug.shape, m_registry, e);

    for (int i = 0; i < model.mesh.size(); ++i) {
      // TODO: create a util for converting to model with entities/components.
      auto debugEntity = m_registry.create();
      std::string debugName = std::string{"Debug#"} + std::to_string(i) + " " + name;
      m_registry.emplace<Name>(debugEntity, debugName);
      m_registry.emplace<izz::gl::Renderable>(debugEntity, model.renderable[i]);
      m_registry.emplace<geo::Mesh>(debugEntity, model.mesh[i]);
      m_registry.emplace<izz::gl::Material>(debugEntity, model.material[i]);
      m_registry.emplace<Transform>(debugEntity, model.transformations[i]);
      RelationshipUtil::MakeChild(m_registry, e, debugEntity);
    }
  }
}
