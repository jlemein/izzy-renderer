//
// Created by jlemein on 07-11-20.
//
#include <izzgl_gammacorrectionpe.h>

#include <ecs_camera.h>
#include <izzgl_gpu.h>
#include <izzgl_materialsystem.h>
#include <izzgl_postprocessor.h>
#include <izzgl_texturesystem.h>
#include "izz_gamma.h"
#include "izzgl_error.h"
#include "izzgl_framebufferbuilder.h"
#include "izzgl_shadersystem.h"
#include "uniform_gammacorrection.h"
using namespace izz::gl;

GammaCorrectionPE::GammaCorrectionPE(std::shared_ptr<Gpu> gpu, Postprocessor& postprocessor, entt::registry& registry)
  : m_gpu{gpu}
  , m_postprocessor{postprocessor}
  , m_registry{registry} {}

void GammaCorrectionPE::init(int width, int height) {
  m_width = width;
  m_height = height;

  //  auto rectangle = izz::geo::PrimitiveFactory::MakePlaneXY("PostprocessRect", 2.0, 2.0);
  //  m_rectangle = m_gpu->meshes.createVertexBuffer(rectangle).id;
  auto& gammaCorrectionMaterial = m_gpu->materials.createMaterial("gamma_correction");
  m_postEffect = gammaCorrectionMaterial.id;

  m_hdrTexture = m_postprocessor.getFramebuffer()->getColorAttachment(0);
//  m_hdrTexture = m_gpu->textures.allocateTexture(m_width, m_height, TextureFormat::RGBA16F);

  FramebufferBuilder builder(*m_gpu);
  builder.setColorAttachment(m_hdrTexture);
  m_framebuffer = builder.create();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  gammaCorrectionMaterial.setTexture("hdrTexture", m_hdrTexture);
  izz::gl::checkError("initPostprocessBuffers");
}

void GammaCorrectionPE::update(float dt, float time) {
  // postprocess
  auto view = m_registry.view<ecs::Camera>();
  if (view.size() > 0) {
    auto e = view[0];
    if (m_registry.all_of<izz::GammaCorrection>(e)) {
      auto& gammaCorrection = m_registry.get<izz::GammaCorrection>(e);
      std::cout << "Setting gamma to " << gammaCorrection.gamma << std::endl;
      auto& material = m_gpu->materials.getMaterialById(m_postEffect);
      auto u = material.getUniformBuffer<izz::ufm::GammaCorrection>();
      u->invGamma = 1.0 / gammaCorrection.gamma;
    }
  }
}

void GammaCorrectionPE::render() {
  m_postprocessor.render(m_postEffect);
//
//  // first copy output to depth and color texture
//  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
//  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//  m_gpu->materials.bindMaterial(m_postEffect);
//  m_gpu->meshes.bindBuffer(m_rectangle);
//  auto& mesh = m_gpu->meshes.getMeshBuffer(m_rectangle);
//  glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
}

void GammaCorrectionPE::resize(int width, int height) {
  m_width = width;
  m_height = height;
  glViewport(0, 0, m_width, m_height);
}
