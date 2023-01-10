//
// Created by jeffrey on 6-1-23.
//
#include <geo_mesh.h>
#include <geo_primitivefactory.h>
#include <izzgl_framebuffer.h>
#include <izzgl_gpu.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgl_meshsystem.h>
#include <izzgl_postprocessor.h>
using namespace izz::gl;

namespace {
auto screenSpaceRect = izz::geo::PrimitiveFactory::MakePlaneXY("screen_space_rect", 2.0, 2.0);
}

Postprocessor::Postprocessor(Gpu& gpu, Framebuffer* tempFramebuffer)
  : m_gpu{gpu} {
  m_tempBuffer = tempFramebuffer;
  if (tempFramebuffer) {
    m_copyTexture = m_tempBuffer->getColorAttachment(0);
  }
}

void Postprocessor::initialize() {
  m_screenRect = m_gpu.meshes.createVertexBuffer(screenSpaceRect).id;
}

void Postprocessor::setFramebuffer(Framebuffer* framebuffer) {
  m_tempBuffer = framebuffer;
  m_copyTexture = m_tempBuffer ? framebuffer->getColorAttachment(0) : nullptr;
}

Framebuffer* Postprocessor::getFramebuffer() const {
  return m_tempBuffer;
}

void Postprocessor::render(Material& material) {
  render(material.id);
}

void Postprocessor::render(MaterialId id) {
  // first copy output to depth and color texture
  // todo: remove this. The user is responsible for setting correct read buffer, otherwise use different function below.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_tempBuffer->bufferId());
  glBlitFramebuffer(0, 0, m_copyTexture->width, m_copyTexture->height, 0, 0, m_copyTexture->width, m_copyTexture->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  m_gpu.materials.bindMaterial(id);
  m_gpu.meshes.bindBuffer(m_screenRect);

  auto& mesh = m_gpu.meshes.getMeshBuffer(m_screenRect);
  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
  glEnable(GL_DEPTH_TEST);
}

void Postprocessor::render(MaterialId materialId, GLuint readFbo, GLuint drawFbo) {
  // first copy output to depth and color texture
  glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_gpu.materials.bindMaterial(materialId);
  m_gpu.meshes.bindBuffer(m_screenRect);

  auto& mesh = m_gpu.meshes.getMeshBuffer(m_screenRect);
  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
}

void Postprocessor::render(Material& material, GLuint readFbo, GLuint drawFbo) {
  render(material.id);
}