//
// Created by jlemein on 18-02-22.
//
#include <gl_deferredrenderablefactory.h>
#include <gl_deferredrenderer.h>
#include <gl_renderutils.h>
#include <gl_rendersystem.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderableFactory::DeferredRenderableFactory(RenderSystem& renderSystem)
: m_renderSystem{renderSystem}
{}

void DeferredRenderableFactory::addRenderableComponent(entt::registry& registry, entt::entity e) {
  const auto& material = registry.get<lsw::geo::Material>(e);
  gl::DeferredRenderable renderable;
  auto& rs = m_renderSystem.createRenderState();
  renderable.renderStateId = rs.id;
//  auto material = materialSystem->createMaterial("table_cloth");
  RenderUtils::LoadMaterial(material, rs);

  // MVP
  renderable.mvp = RenderUtils::GetUniformBufferLocation(rs, "UniformBufferBlock");
  renderable.lights = RenderUtils::GetUniformBufferLocation(rs, "ForwardLighting");

//  renderable.materialId = tableCloth.id;
  registry.emplace<gl::DeferredRenderable>(e, renderable);
}
