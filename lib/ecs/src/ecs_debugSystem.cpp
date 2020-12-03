//
// Created by jlemein on 01-12-20.
//
#include <ecs_debugsystem.h>
#include <ecs_debug.h>
#include <ecs_renderable.h>
#include <shp_primitivefactory.h>

#include <entt/entt.hpp>

using namespace artifax::ecs;

DebugSystem::DebugSystem(entt::registry &registry)
: m_registry{registry}
{
}


void DebugSystem::init() {
  // handling debug meshes
  auto view = m_registry.view<Debug, Renderable>();
//  for (auto e : view) {
//    m_registry
//  }
}

void DebugSystem::preloadDebugAssets()
{
  auto box = shp::PrimitiveFactory::MakeBox(10.0F, 1.0F, 2.0F);
//  EulerArrow euler;
}