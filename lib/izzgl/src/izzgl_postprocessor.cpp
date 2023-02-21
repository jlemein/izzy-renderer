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
#include "izzgl_framebufferbuilder.h"
using namespace izz::gl;

namespace {
auto screenSpaceRect = izz::geo::PrimitiveFactory::MakePlaneXY("screen_space_rect", 2.0, 2.0);
}

Postprocessor::Postprocessor(Gpu* gpu, Framebuffer* pingPongFramebuffer)
  : m_gpu{gpu} {
  m_pingPongBuffer = pingPongFramebuffer;

  if (pingPongFramebuffer) {
    m_pingPongTexture = m_pingPongBuffer->getColorAttachment(0);
  }

  // create ping pong buffer

  // create screen space quad
  m_fullScreenQuad = m_gpu->meshes.createVertexBuffer(screenSpaceRect).id;
}

//void Postprocessor::setFramebuffer(Framebuffer* framebuffer) {
//  m_pingPongBuffer = framebuffer;
//  m_pingPongTexture = m_pingPongBuffer ? framebuffer->getColorAttachment(0) : nullptr;
//}
//
//Framebuffer* Postprocessor::getFramebuffer() const {
//  return m_pingPongBuffer;
//}

void Postprocessor::render(Framebuffer* from, Framebuffer* to, PostEffect posteffect) {
  // bind the from-framebuffer is active.
  from->bindRead(); //  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  // bind the destination
  if (from->bufferId() == to->bufferId()){
    // if buffers are the same, we have a problem. We need to write to a temporary frame buffer. This is less optimal.
    m_pingPongBuffer->bindDestination(); //  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pingPongBuffer->bufferId());
    from->blit(m_pingPongBuffer);
    //  glBlitFramebuffer(0, 0, m_pingPongTexture->width, m_pingPongTexture->height, 0, 0, m_pingPongTexture->width, m_pingPongTexture->height,
    //                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //
    //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  glDisable(GL_DEPTH_TEST);
  } else {
    // there is only posteffect, so we don't need to ping pong.
    to->bindDestination();
  }

  // bind the screen space rectangle
  m_gpu->meshes.bindBuffer(posteffect.vertexBufferId);

  // get material
  auto& material = m_gpu->materials.getMaterialById(posteffect.materialId);

  // bind the texture slots of the posteffect to the correct from-framebuffer textures.
  for (auto [n, name] : posteffect.colorAttachmentMapping) {
    Texture* texture = from->getColorAttachment(n);
    material.setTexture(name, texture);
  }

  if (!posteffect.depthAttachmentMapping.empty()) {
    Texture* texture = from->getDepthAttachment();
    material.setTexture(posteffect.depthAttachmentMapping, texture);
  }

  m_gpu->materials.bindMaterial(posteffect.materialId);

  auto& mesh = m_gpu->meshes.getMeshBuffer(posteffect.vertexBufferId);
  m_gpu->meshes.bindBuffer(mesh.id);

  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
}
//
//void Postprocessor::render(Material& material) {
//  render(material.id);
//}
//
//void Postprocessor::render(MaterialId id) {
//
//
//
//  // first copy output to depth and color texture
//  // todo: remove this. The user is responsible for setting correct read buffer, otherwise use different function below.
//  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pingPongBuffer->bufferId());
//  glBlitFramebuffer(0, 0, m_pingPongTexture->width, m_pingPongTexture->height, 0, 0, m_pingPongTexture->width, m_pingPongTexture->height,
//                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glDisable(GL_DEPTH_TEST);
//
//  m_gpu->materials.bindMaterial(id);
//  m_gpu->meshes.bindBuffer(m_fullScreenQuad);
//
//  auto& mesh = m_gpu->meshes.getMeshBuffer(m_fullScreenQuad);
//  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
//  glEnable(GL_DEPTH_TEST);
//}
//
//void Postprocessor::render(MaterialId materialId, GLuint readFbo, GLuint drawFbo) {
//  // first copy output to depth and color texture
//  glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
//  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//  m_gpu->materials.bindMaterial(materialId);
//  m_gpu->meshes.bindBuffer(m_fullScreenQuad);
//
//  auto& mesh = m_gpu->meshes.getMeshBuffer(m_fullScreenQuad);
//  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
//}
//
//void Postprocessor::render(Material& material, GLuint readFbo, GLuint drawFbo) {
//  render(material.id);
//}