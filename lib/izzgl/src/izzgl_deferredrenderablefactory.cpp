//
// Created by jlemein on 18-02-22.
//
#include <gl_renderutils.h>
#include <izzgl_deferredrenderablefactory.h>
#include <izzgl_deferredrenderer.h>
#include <izzgl_materialsystem.h>
#include <izzgl_rendersystem.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderableFactory::DeferredRenderableFactory(RenderSystem& renderSystem, MaterialSystem& materialSystem)
  : m_renderSystem{renderSystem}
  , m_materialSystem{materialSystem} {}

void DeferredRenderableFactory::addRenderableComponent(entt::registry& registry, entt::entity e, int materialId, int meshBufferId) {
  spdlog::debug("(e: {}) ADD DeferredRenderable; material id: {}, meshBufferId: {}", static_cast<int>(e), materialId, meshBufferId);
  auto& renderable = registry.emplace<gl::DeferredRenderable>(e, gl::DeferredRenderable{materialId, meshBufferId});
}
