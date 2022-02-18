//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <ecs_camera.h>
#include <gl_deferredrenderer.h>
#include <gl_rendersystem.h>
#include <gl_renderutils.h>
#include <izz_scenegraphhelper.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderer::DeferredRenderer(izz::gl::RenderSystem& renderSystem, entt::registry& registry)
  : m_renderSystem{renderSystem}
  , m_registry{registry} {}

void DeferredRenderer::init() {
  const int SCREEN_WIDTH = 600;
  const int SCREEN_HEIGHT = 400;

  // Create a framebuffer for the gbuffer (geometry pass).
  glGenFramebuffers(1, &m_gBufferFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);

  // GBuffer: position texture
  glGenTextures(1, &m_gPosition);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

  // GBuffer: normal texture
  glGenTextures(1, &m_gNormal);
  glBindTexture(GL_TEXTURE_2D, m_gNormal);  // so that all subsequent calls will affect normal texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

  // GBuffer: albedoTexture
  glGenTextures(1, &m_gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);  // so that all subsequent calls will affect albedoSpec texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

  //=======================================
  //  PARSE THE SCENE
  //=======================================

  // handling curves
  for (const auto& [entity, curve, r] : m_registry.view<lsw::geo::Curve, DeferredRenderable>().each()) {
    RenderUtils::FillBufferedMeshData(curve, r.meshData);
  }

  // handling meshes
  for (const auto& [entity, mesh, r] : m_registry.view<lsw::geo::Mesh, DeferredRenderable>().each()) {
    RenderUtils::FillBufferedMeshData(mesh, r.meshData);
  }

  // find mvp matrix
}

void DeferredRenderer::update() {
  // Updates the Render system updates the model view projection matrix for each of the
  // The camera
  for (auto [e, r] : m_registry.view<DeferredRenderable>().each()) {
    auto& rs = m_renderSystem.getRenderState(r.renderStateId);

    if (r.mvp.blockIndex == -1) {
      throw std::runtime_error("Deferred Renderable has no MVP uniform buffer block (index -1)");
    }

    // update model matrix
    auto transform = m_registry.try_get<lsw::ecs::Transform>(e);
    UniformBlock& mvp = reinterpret_cast<UniformBlock&>(r.mvp.pData);
    mvp.model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);

    // update camera
    entt::entity m_activeCamera;
    if (m_activeCamera == entt::null) {
      throw std::runtime_error("No active camera in scene");
    }

    auto& cameraTransform = m_registry.get<lsw::ecs::Transform>(m_activeCamera);
    auto& activeCamera = m_registry.get<lsw::ecs::Camera>(m_activeCamera);

    mvp.view = glm::inverse(cameraTransform.worldTransform);
    mvp.proj = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
    mvp.viewPos = glm::vec3(cameraTransform.worldTransform[3]);
  }
}

void DeferredRenderer::render(const entt::registry& registry) {
  static unsigned int colorAttachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);
  glDrawBuffers(3, colorAttachments);

  // clear gbuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto view = registry.view<const DeferredRenderable, const lsw::ecs::Transform>();
  for (entt::entity e : view) {
    const auto& r = registry.get<const DeferredRenderable>(e);
    const RenderState& rs = m_renderSystem.getRenderState(r.renderStateId);

    //    RenderUtils::ActivateProgram();
    spdlog::info("Program used: {}", rs.program);
    glUseProgram(rs.program);
    RenderUtils::ActivateTextures(rs);
    RenderUtils::UseBufferedMeshData(rs.meshData);

    // TODO: check if shader is dirty
    //  reason: if we push properties every frame (Except for MVP), we might
    //  unnecessary spend time doing that while we can immediately just render.
    RenderUtils::ActivateUniformProperties(rs);

    if (rs.meshData.primitiveType == GL_TRIANGLES) {
      glDrawElements(rs.meshData.primitiveType, rs.meshData.drawElementCount, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(rs.meshData.primitiveType, 0, rs.meshData.drawElementCount);
    }
  }

  // --- GBUFFER pass is finished. GBuffer is what we have -----

  /* We are going to blit into the window (default framebuffer)                     */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK); /* Use backbuffer as color dst.         */

  /* Read from your FBO */
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBufferFbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0); /* Use Color Attachment 0 as color src. */

  int width = 800;
  int height = 600;

  /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}