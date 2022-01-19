//
// Created by jeffrey on 19-01-22.
//
#include <gl_deferredrenderer.h>
#include <entt/entt.hpp>
#include <GL/glew.h>
using namespace izz::gl;

void DeferredRenderer::render(const entt::registry& registry) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}