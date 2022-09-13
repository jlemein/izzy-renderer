//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <ecs_camera.h>
#include <ecs_name.h>
#include <ecs_transform.h>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <gl_deferredrenderer.h>
#include <gl_renderutils.h>
#include <izzgl_rendersystem.h>
#include <entt/entt.hpp>
#include "geo_primitivefactory.h"
#include "uniform_deferredlighting.h"
using namespace izz::gl;

DeferredRenderer::DeferredRenderer(izz::gl::RenderSystem& renderSystem, entt::registry& registry)
  : m_renderSystem{renderSystem}
  , m_registry{registry} {
  //  m_registry.on_construct<gl::DeferredRenderable>().connect<&DeferredRenderer::onConstruct>(this);
}

void DeferredRenderer::onConstruct(entt::registry& registry, entt::entity e) {
  DeferredRenderable& r = registry.get<DeferredRenderable>(e);
  try {
    //    auto& rs = r.renderStateId == -1 ? m_renderSystem.createRenderState() : m_renderSystem.getRenderState(r.renderStateId);
    //    r.renderStateId = rs.id;

    // initialize mesh / curve data
    auto pCurve = registry.try_get<lsw::geo::Curve>(e);
    //    if (pCurve != nullptr) {
    ////      r.materialId = pCurve->materialId;
    ////      RenderUtils::FillBufferedMeshData(*pCurve, rs.meshData);
    //
    //      // initialize material
    ////      if (pCurve->materialId > -1) {
    ////        auto& material = m_renderSystem.getMaterialSystem().getMaterialById(pMesh->materialId);
    ////        RenderUtils::LoadMaterial(material, rs);
    ////      }
    //    }
    //    auto& material = m_renderSystem.getMaterialSystem().getMaterialById(r.materialId);
    //    auto& meshBuffer = m_renderSystem.getMeshSystem().getMeshBuffer(r.meshBufferId);

    //    auto pMesh = registry.try_get<lsw::geo::Mesh>(e);
    //    if (pMesh != nullptr) {
    //      spdlog::info("\tInitializing {}", pMesh->name);
    //      r.materialId = pMesh->materialId;
    //      RenderUtils::FillBufferedMeshData(*pMesh, rs.meshData);
    //
    //      // initialize material
    //      if (pMesh->materialId > -1) {
    //        auto& material = m_renderSystem.getMaterialSystem().getMaterialById(pMesh->materialId);
    ////        RenderUtils::LoadMaterial(material, rs);
    //
    //        spdlog::info("\t[e: {}] Mesh {} initialized - material id {} -- {}",
    //                     static_cast<int>(e), pMesh->name, pMesh->materialId, material.name);
    //      } else {
    //        spdlog::warn("No material assigned to mesh {}", pMesh->name);
    //      }
    //    }
  } catch (std::exception& e) {
    //    auto name = registry.all_of<lsw::ecs::Name>(e) ? m_registry.get<lsw::ecs::Name>(e).name : "Unnamed";
    auto name = "BLABLA";
    throw std::runtime_error(fmt::format("Failed initializing mesh '{}': {}", name, e.what()));
  }

  //
  //  // if render state exists
  //  std::cout << "Hello On construct - renderstateid = " << r.renderStateId << std::endl;
  //  RenderState& rs = r.renderStateId == -1 ? m_renderSystem.createRenderState() : m_renderSystem.getRenderState(r.renderStateId);
  //  r.renderStateId = rs.id;
  //
  //
  //  if (r.meshEntity != entt::null) {
  //    auto pCurve = m_registry.try_get<lsw::geo::Curve>(r.meshEntity);
  //    if (pCurve != nullptr) {
  //      RenderUtils::FillBufferedMeshData(*pCurve, rs.meshData);
  //    }
  //
  //    auto pMesh = m_registry.try_get<lsw::geo::Curve>(r.meshEntity);
  //    if (pMesh != nullptr) {
  //      RenderUtils::FillBufferedMeshData(*pMesh, rs.meshData);
  //    }
  //  }
  //
  //  if (r.materialId != -1) {
  //    auto& material = m_renderSystem.getMaterialSystem().getMaterialById(r.materialId);
  //    RenderUtils::LoadMaterial(material, rs);
  //    // MVP
  //    r.mvp = RenderUtils::GetUniformBufferLocation(rs, "UniformBufferBlock");
  //    //    r.lights = RenderUtils::GetUniformBufferLocation(rs, "ForwardLighting");
  //  } else {
  //    spdlog::warn("DeferredRenderable is incomplete. Does not contain material properties.");
  //  }
}

// void DeferredRenderer::resize(int width, int height) {
//   m_screenWidth = width;
//   m_screenHeight = height;
// }

namespace {
void checkError(const char* name) {
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error occurred for " << name << ": " << err << std::endl;
  }
}
}  // namespace
void DeferredRenderer::createGBuffer(int width, int height) {
  // Create a framebuffer for the gbuffer (geometry pass).
  glGenFramebuffers(1, &m_gBufferFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);

  // GBuffer: position texture
  glGenTextures(1, &m_gPosition);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

  // GBuffer: normal texture
  glGenTextures(1, &m_gNormal);
  glBindTexture(GL_TEXTURE_2D, m_gNormal);  // so that all subsequent calls will affect normal texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, m_gNormal, 0);

  // GBuffer: tangent texture
  glGenTextures(1, &m_gTangent);
  glBindTexture(GL_TEXTURE_2D, m_gTangent);  // so that all subsequent calls will affect normal texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, m_gTangent, 0);

  // GBuffer: albedoTexture
  glGenTextures(1, &m_gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);  // so that all subsequent calls will affect albedoSpec texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 3, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

  glGenRenderbuffers(1, &m_depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    spdlog::info("Deferred renderer: successfully created framebuffer");
  } else {
    throw std::runtime_error("Deferred renderer: failed creating a valid frame buffer");
  }
}

void DeferredRenderer::createScreenSpaceRect() {
  auto rectangle = lsw::geo::PrimitiveFactory::MakePlaneXY("ScreenSpaceRect", 2.0, 2.0);
  const auto& meshBuffer = m_renderSystem.getMeshSystem().createMeshBuffer(rectangle);

  // DeferredLightingPass UBO is required.
  const auto& material = m_renderSystem.getMaterialSystem().createMaterial("DeferredLightingPass");
  if (material.uniformBuffers.count("DeferredLighting") == 0) {
    throw std::runtime_error(fmt::format("{}: material '{}' misses required uniform buffer '{}'", ID, material.name, "DeferredLighting"));
  }

  //  const auto& material = m_renderSystem.getMaterialSystem().createMaterial("VisualizeGBuffer");
  m_screenSpaceMaterial = material.getId();
  m_screenSpaceMeshBufferId = meshBuffer.id;

  m_gPositionLoc = glGetUniformLocation(material.programId, "gbuffer_position");
  m_gNormalLoc = glGetUniformLocation(material.programId, "gbuffer_normal");
  m_gTangentLoc = glGetUniformLocation(material.programId, "gbuffer_tangent");
  m_gAlbedoSpecLoc = glGetUniformLocation(material.programId, "gbuffer_albedospec");
}

void DeferredRenderer::init(int width, int height) {
  m_screenWidth = width;
  m_screenHeight = height;

  createGBuffer(m_screenWidth, m_screenHeight);
  createScreenSpaceRect();

  // handling curves
  //  for (auto [entity, curve, r] : m_registry.view<lsw::geo::Curve, DeferredRenderable>().each()) {
  //    try {
  //      RenderUtils::FillBufferedMeshData(curve, m_renderSystem.getRenderState(r.renderStateId).meshData);
  //    } catch (std::exception& e) {
  //      auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
  //      throw std::runtime_error(fmt::format("Failed initializing curve '{}': {}", name, e.what()));
  //    }
  //  }

  // handling meshes
  spdlog::info("Deferred Renderer: Mesh Initialization");
  auto numRenderables = m_registry.view<DeferredRenderable>().size();
  auto numMeshRenderables = m_registry.view<lsw::geo::Mesh, DeferredRenderable>().size_hint();

  if (numRenderables != numMeshRenderables) {
    spdlog::warn("It seems some renderable objects are missing a mesh: renderables: {} vs renderables w. mesh: {}", numRenderables, numMeshRenderables);
  }

  for (auto [entity, mesh, r] : m_registry.view<lsw::geo::Mesh, DeferredRenderable>().each()) {
    onConstruct(m_registry, entity);
  }

  spdlog::info("=== Deferred Renderer Initialization Complete ===");
}

void DeferredRenderer::update() {
  // Updates the Render system updates the model view projection matrix for each of the
  if (m_activeCamera == entt::null) {
    throw std::runtime_error("No active camera in scene");
  }

  auto& cameraTransform = m_registry.get<lsw::ecs::Transform>(m_activeCamera);
  glm::mat4 view = glm::inverse(cameraTransform.worldTransform);

  auto& activeCamera = m_registry.get<lsw::ecs::Camera>(m_activeCamera);
  glm::mat4 proj = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);

  for (auto [e, r] : m_registry.view<DeferredRenderable>().each()) {
    auto transform = m_registry.try_get<lsw::ecs::Transform>(e);
    auto model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);

    try {
      auto& material = m_renderSystem.getMaterialSystem().getMaterialById(r.materialId);
      ModelViewProjection* mvp = reinterpret_cast<ModelViewProjection*>(material.uniformBuffers.at("ModelViewProjection").data);
      mvp->model = model;
      mvp->view = view;
      mvp->proj = proj;

      // camera position is stored in 4-th column of inverse view matrix.
      mvp->viewPos = glm::inverse(view)[3];

    } catch (std::out_of_range&) {
      auto materialName = m_renderSystem.getMaterialSystem().getMaterialById(r.materialId).getName();
      auto nameComponent = m_registry.try_get<lsw::ecs::Name>(e);
      auto name = (nameComponent != nullptr) ? nameComponent->name : "<unnamed>";
      throw std::runtime_error(
          fmt::format("{}: e:{} ({}): cannot access ModelViewProjection matrix for material '{}'. Does shader have 'ModelViewProjection' uniform buffer?",
                      ID, e, name, materialName));
    } catch (std::runtime_error error) {
      auto nameComponent = m_registry.try_get<lsw::ecs::Name>(e);
      auto name = (nameComponent != nullptr) ? nameComponent->name : "<unnamed>";
      throw std::runtime_error(fmt::format("{}: e:{} ({}): {}", ID, e, name, error.what()));
    }
  }

  auto& lightingMaterial = m_renderSystem.getMaterialSystem().getMaterialById(m_screenSpaceMaterial);
  izz::ufm::DeferredLighting* deferred = reinterpret_cast<izz::ufm::DeferredLighting*>(lightingMaterial.uniformBuffers.at("DeferredLighting").data);

  deferred->numberOfLights = 1;
  deferred->viewPos = view[3];
  for (int i=0; i<25; i++) {
    deferred->pointLights[i].color = glm::vec4(1, 1, 1, 0);
    deferred->pointLights[i].position = glm::vec4(0.5f,2.f,0.f, 0.f); // glm::vec4(2*(i%5 - 2.5), .4, 2*(-2.5 + i/5), 0);
    deferred->pointLights[i].radius = 1.0;
    deferred->pointLights[i].intensity = 1.0;
  }
}

void DeferredRenderer::render(const entt::registry& registry) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);
  static unsigned int colorAttachments[] = {
      GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
  };
  int attachmentCount = sizeof(colorAttachments) / sizeof(unsigned int);
  glDrawBuffers(attachmentCount, colorAttachments);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto view = registry.view<const DeferredRenderable, const lsw::ecs::Transform>();

  for (entt::entity e : view) {
    try {
      auto name = registry.get<lsw::ecs::Name>(e);

      auto deferred = view.get<const DeferredRenderable>(e);
      auto materialId = deferred.materialId;
      const auto& mat = m_renderSystem.getMaterialSystem().getMaterialById(materialId);

      auto meshBufferId = deferred.meshBufferId;
      const auto& mesh = m_renderSystem.getMeshSystem().getMeshBuffer(meshBufferId);
      
      glUseProgram(mat.programId);

      mat.useTextures();
      mat.pushUniforms();
      m_renderSystem.getMeshSystem().bindBuffer(mesh);
      // TODO: check if shader is dirty
      //  reason: if we push properties every frame (Except for MVP), we might
      //  unnecessary spend time doing that while we can immediately just render.

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      // 1. Render data to G-buffer
      if (mesh.primitiveType == GL_TRIANGLES) {
        glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
      } else {
        glDrawArrays(mesh.primitiveType, 0, mesh.drawElementCount);
      }
    } catch (std::exception& exc) {
      std::string msg = "";

      if (registry.all_of<lsw::ecs::Name>(e)) {
        auto& name = registry.get<lsw::ecs::Name>(e);
        msg = fmt::format("(e: {}) Rendering entity: {} - {}", static_cast<int>(e), name.name, exc.what());
      } else {
        msg = exc.what();
      }
      throw std::runtime_error(msg);
    }
  }
  //
  // --- GBUFFER pass is finished. GBuffer is what we have -----
  //
  /* We are going to blit into the window (default framebuffer)                     */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK); /* Use backbuffer as color dst.         */

  /* Read from your FBO */
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBufferFbo);
  glReadBuffer(GL_COLOR_ATTACHMENT1); /* Use Color Attachment 0 as color src. */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);

  auto& material = m_renderSystem.getMaterialSystem().getMaterialById(m_screenSpaceMaterial);

  glUseProgram(material.programId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);
  glUniform1i(m_gPositionLoc, 0);

  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, m_gNormal);
  glUniform1i(m_gNormalLoc, 1);

  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, m_gTangent);
  glUniform1i(m_gTangentLoc, 2);

  glActiveTexture(GL_TEXTURE0 + 3);
  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
  glUniform1i(m_gAlbedoSpecLoc, 3);

  auto& meshBuffer = m_renderSystem.getMeshSystem().getMeshBuffer(m_screenSpaceMeshBufferId);
  m_renderSystem.getMeshSystem().bindBuffer(meshBuffer);
  material.pushUniforms();
  //    RenderUtils::PushUniformProperties(rs);
  glDrawElements(meshBuffer.primitiveType, meshBuffer.drawElementCount, GL_UNSIGNED_INT, 0);

  //  /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  //    glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
