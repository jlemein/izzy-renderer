//
// Created by jlemein on 02-12-21.
//
#include <glrs_common.h>
#include <glrs_multipassframebuffer.h>
#include <glrs_shadersystem.h>
#include <iostream>
#include <vector>
using namespace lsw;
using namespace lsw::glrs;

namespace {
void checkError(const char* name) {
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error occurred for " << name << ": " << err << std::endl;
  }
}
}  // namespace

MultipassFramebuffer::MultipassFramebuffer(int numSamplesMSAA)
  : m_numSamplesMSAA{numSamplesMSAA} {}

void MultipassFramebuffer::resize(int width, int height) {
  m_width = width;
  m_height = height;

  spdlog::debug("Framebuffer resizes to {} x {}", width, height);

  // MSAA framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, m_msFbo);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureMSAA);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_numSamplesMSAA, GL_RGB, m_width, m_height, GL_TRUE);
  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_numSamplesMSAA, GL_DEPTH_COMPONENT, width, height);

  // intermediate framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFbo);
  glBindTexture(GL_TEXTURE_2D, m_textureMSAA);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultipassFramebuffer::initialize() {
  GLint maxColorSamples{0}, maxDepthSamples{0}, maxRenderBufferSize{0};
  glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &maxColorSamples);
  glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &maxDepthSamples);
  glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
  spdlog::debug("Maximum render buffer size: {}", maxRenderBufferSize);
  spdlog::debug("Maximum color samples: {}, maximum depth samples: {}", maxColorSamples, maxDepthSamples);

  // prepare quad for collecting
  float vertices[] = {// pos        // tex
                      -1.0f, -1.f, 0.0f, 0.0f, 1.f, 1.f,  1.0f, 1.0f, -1.f, 1.f, 0.0f, 1.0f,

                      -1.f,  -1.f, 0.0f, 0.0f, 1.f, -1.f, 1.0f, 0.0f, 1.f,  1.f, 1.0f, 1.0f};

  // vao 1
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + 4, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(m_vertexAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(m_uvAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(2 * sizeof(float)));

  ShaderSystem m_shaderSystem;
  m_program = m_shaderSystem.compileShader("shaders/postprocess/copybuffer.vert", "shaders/postprocess/copybuffer.frag");

  createMsaaFramebuffer();
  checkError("MSAA Framebuffer");

  createIntermediaFbo();
  checkError("Intermediate framebuffer");

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultipassFramebuffer::createMsaaFramebuffer() {
  glGenTextures(1, &m_textureMSAA);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureMSAA);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_numSamplesMSAA, GL_RGB, m_width, m_height, GL_TRUE);

  glGenRenderbuffers(1, &m_renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_numSamplesMSAA, GL_DEPTH_COMPONENT, m_width, m_height);

  glGenFramebuffers(1, &m_msFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_msFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureMSAA, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created MSAA framebuffer");
  } else {
    throw std::runtime_error("Failed creating a valid MSAA frame buffer");
  }
}

void MultipassFramebuffer::createIntermediaFbo() {
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenFramebuffers(1, &m_intermediateFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created intermediate framebuffer");
  } else {
    throw std::runtime_error("Failed creating a valid intermediate frame buffer");
  }
}

void MultipassFramebuffer::bindFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_msFbo);
}

void MultipassFramebuffer::nextPass() {
  // blit renderbuffer to texbuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateFbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  // bind render framebuffer again
  glBindFramebuffer(GL_FRAMEBUFFER, m_msFbo);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(m_vao);  // make the vertex descriptor of vbo active

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  //
  //  glBindVertexArray(0);

  // bind the texture from the intermediate fbo//
//  glDrawArrays(GL_TRIANGLES, 0, 6);
//
//  glBindVertexArray(0);
}

void MultipassFramebuffer::finish() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  checkError("A");
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

//  // finished rendering off-screen.
//  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msFbo);
//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediateFbo);
//  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//  glUseProgram(m_program);
//  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//  glBindVertexArray(m_vao);  // make the vertex descriptor of vbo active
//
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D, m_texture);
//
//  glDrawArrays(GL_TRIANGLES, 0, 6);
//
//  glBindVertexArray(0);
}