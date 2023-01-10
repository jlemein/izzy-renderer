//
// Created by jeffrey on 10-1-23.
//
#include <GL/glew.h>
#include <izzgl_framebufferbuilder.h>
#include <izzgl_gpu.h>
#include <izzgl_framebuffer.h>
#include <izz_texture.h>
using namespace izz::gl;

FramebufferBuilder::FramebufferBuilder(izz::gl::Gpu& gpu)
  : m_gpu{gpu}
  , m_depthAttachment{nullptr} {}

FramebufferBuilder& FramebufferBuilder::setColorAttachment(Texture* texture) {
  return setColorAttachment(0, texture);
}

FramebufferBuilder& FramebufferBuilder::setColorAttachment(int attachment, Texture* texture) {
  assert(attachment >= 0 && attachment <= 6);  // OpenGL goes as far as GL_COLOR_ATTACHMENT15
  m_colorAttachments[attachment] = texture;
  return *this;
}

FramebufferBuilder& FramebufferBuilder::setDepthAttachment(Texture* texture) {
  m_depthAttachment = texture;
  return *this;
}

Framebuffer* FramebufferBuilder::create() {
  Framebuffer* framebuffer = m_gpu.allocateFramebuffer();
  framebuffer->bind();

  // set the color attachments
  for (auto [attachment, texture] : m_colorAttachments) {
    if (texture->type != TextureType::TEXTURE_2D) {
      throw std::runtime_error("Cannot set color attachment of framebufferColor attachment ");
    }

    framebuffer->setColorAttachment(attachment, texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture->bufferId, 0);
  }

  // set the depth attachment (if available)
  if (m_depthAttachment) {
    if (m_depthAttachment->type == TextureType::TEXTURE_2D) {
      framebuffer->setDepthAttachment(m_depthAttachment);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment->bufferId, 0);
    } else {
      throw std::runtime_error(
          "Cannot attach depth attachment using specified texture. Texture type is invalid. Maybe it is of type renderbuffer (not supported yet).");
    }
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Created framebuffer is invalid");
  }

  return framebuffer;
}