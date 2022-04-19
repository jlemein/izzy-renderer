//
// Created by jlemein on 18-02-22.
//
#include <gl_deferredrenderablefactory.h>
#include <gl_deferredrenderer.h>
#include <gl_renderutils.h>
#include <izzgl_materialsystem.h>
#include <izzgl_rendersystem.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderableFactory::DeferredRenderableFactory(RenderSystem& renderSystem, MaterialSystem& materialSystem)
: m_renderSystem{renderSystem}, m_materialSystem{materialSystem}
{}

void DeferredRenderableFactory::addRenderableComponent(entt::registry& registry, entt::entity e, int materialId) {

//  const auto& material = registry.get<lsw::geo::Material>(e);
//  gl::DeferredRenderable renderable;
  auto& rs = m_renderSystem.createRenderState();
  spdlog::debug("(e: {}) ADD DeferredRenderable; renderState id: {}, material id: {}", static_cast<int>(e), rs.id, materialId);
  auto& renderable = registry.emplace<gl::DeferredRenderable>(e, gl::DeferredRenderable{rs.id, materialId});

//  renderable.renderStateId = rs.id;
//  renderable.materialId = materialId;

  auto& material = m_materialSystem.getMaterialById(materialId);
  RenderUtils::LoadMaterial(material, rs);

  // MVP
//  renderable.mvp = RenderUtils::GetUniformBufferLocation(rs, "UniformBufferBlock");
//  renderable.lights = RenderUtils::GetUniformBufferLocation(rs, "ForwardLighting");
//  registry.emplace<gl::DeferredRenderable>(e, renderable);
//


//  registry.emplace_or_replace<PP>(e);
}
