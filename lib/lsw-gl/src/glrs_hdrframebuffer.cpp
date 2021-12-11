//
// Created by jlemein on 02-12-21.
//
//#include <glrs_common.h>
#include <glrs_hdrframebuffer.h>
#include <iostream>
#include <spdlog/spdlog.h>
using namespace lsw;
using namespace lsw::glrs;

HdrFramebuffer::HdrFramebuffer(int width, int height, int numSamplesMSAA)
  : m_width(width)
  , m_height(height)
  , m_numSamplesMSAA{numSamplesMSAA} {}

void HdrFramebuffer::setSize(int width, int height) {
  m_width = width;
  m_height = height;
}

void HdrFramebuffer::initialize() {
  glGenTextures(1, &m_hdrTexture);
  glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrTexture, 0);
  // depth component
  glGenRenderbuffers(1, &m_renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created HDR framebuffer");
  } else {
    throw std::runtime_error("Failed creating a valid MSAA frame buffer");
  }

  // intermediate hdr framebuffer
  glGenFramebuffers(1, &m_fbo1);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo1);
  // color attachment 0
  glGenTextures(1, &m_intermediateHdr);
  glBindTexture(GL_TEXTURE_2D, m_intermediateHdr);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_intermediateHdr, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created intermediate HDR framebuffer");
  } else {
    throw std::runtime_error("Failed creating a valid intermediate HDR frame buffer");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // depth component
//  glGenRenderbuffers(1, &m_renderbuffer);
//  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
//  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
//  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);


//  createMsaaFramebuffer();
//  izz::gl::checkError("MSAA Framebuffer");

//  createIntermediaFbo();
//  izz::gl::checkError("Intermediate framebuffer");
}

void HdrFramebuffer::resize(int width, int height) {
  m_width = width;
  m_height = height;

//  // ---- render buffer --------------------------------
//  glBindFramebuffer(GL_FRAMEBUFFER, m_msFbo);
//  glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
//  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
//  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//
//  // intermediate framebuffer
//  glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFbo);
//  glBindTexture(GL_TEXTURE_2D, m_texture);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
//
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HdrFramebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void HdrFramebuffer::nextPass() {
  // blit renderbuffer to texture buffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);  // read from scene drawing
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo1); // bind the intermediate buffer
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_intermediateHdr);

  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); // render to the scene again


}

void HdrFramebuffer::apply() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}