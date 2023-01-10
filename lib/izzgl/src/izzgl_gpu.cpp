//
// Created by jeffrey on 6-1-23.
//
#include <GL/glew.h>
#include <izz_texture.h>
#include <izzgl_framebuffer.h>
#include <izzgl_gpu.h>
using namespace izz::gl;

Gpu::Gpu(izz::gl::MaterialSystem& materialSystem, izz::gl::TextureSystem& textureSystem, izz::gl::MeshSystem& meshSystem)
  : materials{materialSystem}
  , textures{textureSystem}
  , meshes{meshSystem}
  , m_defaultFramebuffer(new Framebuffer(*this, 0))
{
  m_activeFramebuffer = m_defaultFramebuffer;
}

Gpu::~Gpu() {
  delete m_defaultFramebuffer;
}

void Gpu::bindFramebuffer(Framebuffer* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->bufferId());
}

void Gpu::bindReadFramebuffer(Framebuffer* framebuffer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->bufferId());
}

void Gpu::bindDrawFramebuffer(Framebuffer* framebuffer) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->bufferId());
}


Framebuffer* Gpu::allocateFramebuffer() {
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  Framebuffer framebuffer(*this, fbo);

  m_framebuffers.push_back(framebuffer);
  return &m_framebuffers.back();
}
