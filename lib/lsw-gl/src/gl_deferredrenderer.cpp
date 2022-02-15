//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <gl_deferredrenderer.h>
#include <gl_rendersystem.h>
#include <gl_renderutils.h>
#include <izz_scenegraph.h>
#include <entt/entt.hpp>
using namespace izz::gl;

DeferredRenderer::DeferredRenderer(izz::gl::RenderSystem& renderSystem, std::shared_ptr<izz::SceneGraph> sceneGraph)
  : m_renderSystem{renderSystem}
  , m_sceneGraph{sceneGraph} {}

void DeferredRenderer::init() {
  const int SCREEN_WIDTH = 600;
  const int SCREEN_HEIGHT = 400;

  // Create a framebuffer for the gbuffer (geometry pass).
  glGenFramebuffers(1, &m_gBufferFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);

  // GBuffer: position texture
  glGenTextures(GL_TEXTURE_2D, &m_gPosition);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

  // GBuffer: normal texture
  glGenTextures(GL_TEXTURE_2D, &m_gNormal);
  glBindTexture(GL_TEXTURE_2D, m_gNormal);  // so that all subsequent calls will affect normal texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

  // GBuffer: albedoTexture
  glGenTextures(GL_TEXTURE_2D, &m_gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);  // so that all subsequent calls will affect albedoSpec texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

  //=======================================
  //  PARSE THE SCENE
  //=======================================
//  for (auto e : m_sceneGraph->getRegistry().view<DeferredRenderable>()) {
//    auto& r = m_sceneGraph->getRegistry().get<DeferredRenderable>(e);
//
//    if (m_sceneGraph->getRegistry().all_of<lsw::geo::Curve>(e)) {
//      const auto& curve = m_sceneGraph->getRegistry().get<lsw::geo::Curve>(e);
//      RenderUtils::FillBufferedMeshData(curve, r.meshData);
//    }
//
//  }
  // handling curves
  for (const auto& [entity, curve, r] : m_sceneGraph->getRegistry().view<lsw::geo::Curve, DeferredRenderable>().each()) {
    RenderUtils::FillBufferedMeshData(curve, r.meshData);
  }

  // handling meshes
  for (const auto& [entity, mesh, r] : m_sceneGraph->getRegistry().view<lsw::geo::Mesh, DeferredRenderable>().each()) {
    RenderUtils::FillBufferedMeshData(mesh, r.meshData);
  }
}

void DeferredRenderer::render(const entt::registry& registry) {
  static unsigned int colorAttachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);
  glDrawBuffers(3, colorAttachments);

  auto view = registry.view<const DeferredRenderable, const lsw::ecs::Transform>();
  for (entt::entity e : view) {
    const auto& r = registry.get<const DeferredRenderable>(e);
    const RenderState& rs = m_renderSystem.getRenderState(r.renderStateId);

//    RenderUtils::ActivateProgram();
    glUseProgram(rs.program);
    RenderUtils::ActivateTextures(rs);
    RenderUtils::UseBufferedMeshData(rs.meshData);

    // TODO: check if shader is dirty
    //  reason: if we push properties every frame (Except for MVP), we might
    //  unnecessary spend time doing that while we can immediately just render.
    m_renderSystem.pushShaderProperties(rs);
    if (rs.isMvpSupported) {
      m_renderSystem.pushModelViewProjection(rs);
    }

    // TODO this one needs to change per glUseProgram, which is the case right
    //  now. In future we might optimize changing of glUseProgram in that
    //  case, this function should be called once per set of glUseProgram.
    if (rs.isLightingSupported) {
      m_renderSystem.pushLightingData(rs);
    }

    if (rs.meshData.primitiveType == GL_TRIANGLES) {
      glDrawElements(rs.meshData.primitiveType, rs.meshData.drawElementCount, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(rs.meshData.primitiveType, 0, rs.meshData.drawElementCount);
    }
  }

  // --- GBUFFER pass is finished. GBuffer is what we have -----

  /* We are going to blit into the window (default framebuffer)                     */
  glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer      (GL_BACK);              /* Use backbuffer as color dst.         */

  /* Read from your FBO */
  glBindFramebuffer (GL_READ_FRAMEBUFFER, m_gBufferFbo);
  glReadBuffer      (GL_COLOR_ATTACHMENT0); /* Use Color Attachment 0 as color src. */

  int width = 800;
  int height = 600;

  /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  glBlitFramebuffer (0,0, width,height,
                    0,0, width,height,
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                    GL_NEAREST);
}