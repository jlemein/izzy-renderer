//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <gl_deferredrenderer.h>
#include <glrs_rendersystem.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderer::DeferredRenderer(const lsw::glrs::RenderSystem& renderSystem)
  : m_renderSystem{renderSystem} {}

void DeferredRenderer::render(const entt::registry& registry) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}