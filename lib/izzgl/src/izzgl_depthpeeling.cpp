//
// Created by jeffrey on 14-10-22.
//
#include <GL/glew.h>
#include <ecs_camera.h>
#include <ecs_wireframe.h>
#include <geo_mesh.h>
#include <geo_primitivefactory.h>
#include <izzgl_depthpeeling.h>
#include <izzgl_error.h>
#include <izzgl_materialsystem.h>
#include <izzgl_meshsystem.h>
#include <izzgl_texturesystem.h>
#include <spdlog/spdlog.h>
#include <uniform_depthpeeling.h>
#include "izzgl_gpu.h"
using namespace izz::gl;

DepthPeeling::DepthPeeling(std::shared_ptr<Gpu> gpu, const entt::registry& registry, int numPeelPasses)
  : m_gpu{gpu}
  , m_registry{registry}
  , m_numPeelPasses{numPeelPasses} {}

void DepthPeeling::init(int width, int height) {
  m_width = width;
  m_height = height;

  m_opaqueTexture = m_gpu->textures.allocateTexture(width, height);
  m_depthTexture = m_gpu->textures.allocateDepthTexture(width, height);

  createScreenSpaceRect();

  glGenFramebuffers(1, &m_peelFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_peelFbo);
  glGenTextures(1, &m_peelColor);
  glBindTexture(GL_TEXTURE_2D, m_peelColor);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_peelColor, 0);
  glGenTextures(1, &m_peelDepth);
  glBindTexture(GL_TEXTURE_2D, m_peelDepth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_peelDepth, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created peel1 framebuffer (depth peeling)");
  } else {
    throw std::runtime_error("Failed creating peel1 frame buffer (depth peeling)");
  }

  // Color texture
  glGenTextures(1, &m_peelPrevColor);
  glBindTexture(GL_TEXTURE_2D, m_peelPrevColor);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenTextures(1, &m_peelPrevDepth);
  glBindTexture(GL_TEXTURE_2D, m_peelPrevDepth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Create a framebuffer for the gbuffer (geometry pass).
  glGenFramebuffers(1, &m_opaqueFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_opaqueFbo);

  // Color texture
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_opaqueTexture->bufferId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->bufferId, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Successfully created opaque framebuffer (depth peeling)");
  } else {
    throw std::runtime_error("Failed creating opaque frame buffer (depth peeling)");
  }
}

void DepthPeeling::createScreenSpaceRect() {
  auto rectangle = izz::geo::PrimitiveFactory::MakePlaneXY("ScreenSpaceRect", 2.0, 2.0);
  const auto& meshBuffer = m_gpu->meshes.createVertexBuffer(rectangle);

  // DeferredLightingPass UBO is required.
  const auto& material = m_gpu->materials.createMaterial("RenderTexture");
  m_renderTextureMaterialId = material.getId();
  m_screenSpaceMeshBufferId = meshBuffer.id;
}

void DepthPeeling::render() {
  // copy the depth texture for opaque objects.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_opaqueFbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_peelFbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  // Pass 0: First pass render transparent objects
  glBindFramebuffer(GL_FRAMEBUFFER, m_peelFbo);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  renderTransparentObjects(false);

  for (int i = 0; i < m_numPeelPasses; ++i) {
    // store color/depth buffer of previous render pass.
    glCopyImageSubData(m_peelColor, GL_TEXTURE_2D, 0, 0, 0, 0, m_peelPrevColor, GL_TEXTURE_2D, 0, 0, 0, 0, m_width, m_height, 1);
    glCopyImageSubData(m_peelDepth, GL_TEXTURE_2D, 0, 0, 0, 0, m_peelPrevDepth, GL_TEXTURE_2D, 0, 0, 0, 0, m_width, m_height, 1);

    // next pass: only clear depth buffer.
    // color buffer accumulates contributions over multiple passes.
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture->bufferId);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_peelPrevDepth);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, m_peelPrevColor);
    renderTransparentObjects(true);
  }

  const auto& m = m_gpu->materials.getMaterialById(m_renderTextureMaterialId);
  m.useProgram();
  m.pushUniforms();
  m.useTextures();
  auto& meshBuffer = m_gpu->meshes.getMeshBuffer(m_screenSpaceMeshBufferId);
  m_gpu->meshes.bindBuffer(meshBuffer);

  // combine the opaque and transparent layer.
  // transparent layer is stored in framebuffer (hence destination).
  glDisable(GL_DEPTH_TEST); // depth test is irrelevant. We want to blend two layers on top of each other.
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_opaqueTexture->bufferId);
  glDrawElements(meshBuffer.primitiveType, meshBuffer.drawElementCount, GL_UNSIGNED_INT, 0);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void DepthPeeling::renderTransparentObjects(bool isDepthPeeling) {
  glDisable(GL_CULL_FACE);

  IZZ_STAT_RESET_COUNT(TRANSPARENT_OBJECTS)

  const auto view = m_registry.view<const ForwardRenderable, const izz::ecs::Transform>();

  for (const auto& [e, forward, transform] : view.each()) {
    if (forward.blendMode == izz::BlendMode::ALPHA_BLEND) {
      IZZ_STAT_COUNT(TRANSPARENT_OBJECTS)
      auto& mat = m_gpu->materials.getMaterialById(forward.materialId);
      const auto& mesh = m_gpu->meshes.getMeshBuffer(forward.vertexBufferId);

      auto depthPeeling = mat.getUniformBuffer<izz::ufm::DepthPeeling>();
      depthPeeling->isPeelingPass = static_cast<GLint>(isDepthPeeling);
      depthPeeling->screenSize = glm::ivec2(m_width, m_height);

      m_gpu->materials.updateUniformsForEntity(e, mat);

      mat.useProgram();
      mat.useTextures();
      mat.pushUniforms();
      m_gpu->meshes.bindBuffer(mesh);

      bool isWireframe = forward.isWireframe || m_registry.any_of<izz::ecs::Wireframe>(e);
      glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);

      if (mesh.primitiveType == GL_TRIANGLES) {
        glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
      } else {
        glDrawArrays(mesh.primitiveType, 0, mesh.drawElementCount);
      }

      izz::gl::checkError("Forward renderer");
    }
  }
}
