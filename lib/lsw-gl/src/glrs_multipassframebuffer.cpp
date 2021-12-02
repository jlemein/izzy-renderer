//
// Created by jlemein on 02-12-21.
//
#include <glrs_common.h>
#include <glrs_multipassframebuffer.h>
#include <glrs_shadersystem.h>
#include <vector>

using namespace lsw;
using namespace lsw::glrs;

MultipassFramebuffer::MultipassFramebuffer() {}

void MultipassFramebuffer::initialize() {
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  // prepare quad for collecting
  float vertices[] = {
      // pos        // tex
      -.3f, -.3f, 0.0f, 0.0f,
      .3f,  .3f, 1.0f, 1.0f,
      -.3f,  .3f, 0.0f, 1.0f,

      -.3f, -.3f, 0.0f, 0.0f,
      .3f, -.3f, 1.0f, 0.0f,
      .3f,  .3f, 1.0f, 1.0f
  };
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(m_vertexAttrib);
  glEnableVertexAttribArray(m_vertexAttrib);
  glEnableVertexAttribArray(m_uvAttrib);
  glVertexAttribPointer(m_vertexAttrib, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), izz::gl::BUFFER_OFFSET(0));
  glVertexAttribPointer(m_uvAttrib, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), izz::gl::BUFFER_OFFSET(2*sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  ShaderSystem m_shaderSystem;
  m_program = m_shaderSystem.compileShader("shaders/postprocess/copybuffer.vert", "shaders/postprocess/copybuffer.frag");

  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, 800, 600);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  //  unsigned int rbo;
  //  glGenRenderbuffers(1, &rbo);
  //  glBindRenderbuffer(GL_RENDERBUFFER, rbo);

  // a frame buffer holds a collection of logical.
  //  We have to attach at least one buffer (color, depth or stencil buffer).
  //      There should be at least one color attachment.
  //      All attachments should be complete as well (reserved memory).
  //      Each buffer should have the same number of samples.
  //  glFramebufferRenderbuffer
  //  glFramebufferTexture
  //  glFramebufferTextureLayer

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Succesfully bound framebuffer");
  } else {
    throw std::runtime_error("Failed creating a valid frame buffer");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultipassFramebuffer::bindFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void MultipassFramebuffer::finish() {
  // finished rendering off-screen.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(m_program);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindBuffer(GL_INDEX_ARRAY, 0);

  glBindVertexArray(m_vao);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}