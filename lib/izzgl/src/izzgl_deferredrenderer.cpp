//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <ecs_camera.h>
#include <ecs_transform.h>
#include <geo_mesh.h>
#include <gl_renderutils.h>
#include <izz_util.h>
#include <izzgl_deferredrenderer.h>
#include <izzgl_rendersystem.h>
#include <izzgl_texturesystem.h>
#include "geo_primitivefactory.h"
#include "izz_skybox.h"
#include "izzgl_error.h"
using namespace izz::gl;

DeferredRenderer::DeferredRenderer(std::shared_ptr<MaterialSystem> materialSystem, std::shared_ptr<TextureSystem> textureSystem,
                                   std::shared_ptr<MeshSystem> meshSystem, entt::registry& registry)
  : m_materialSystem{materialSystem}
  , m_textureSystem{textureSystem}
  , m_meshSystem{meshSystem}
  , m_registry{registry} {
  m_registry.on_construct<gl::DeferredRenderable>().connect<&DeferredRenderer::onConstruct>(this);
}

void DeferredRenderer::onConstruct(entt::registry& registry, entt::entity e) {
  // if deferred entity has no renderable component, then it will be added.
  // we need the renderable for the common rendering functionalities (such as mvp updates).
  if (!m_registry.all_of<izz::Geometry>(e)) {
    auto name = m_registry.try_get<izz::Name>(e);
    spdlog::info("DeferredRenderable component added to entity '{}'. Adding Renderable.", name ? name->name : "");

    auto deferred = m_registry.get<gl::DeferredRenderable>(e);
    auto renderable = izz::Geometry{.materialId = deferred.materialId, .vertexBufferId = deferred.vertexBufferId};
    m_registry.emplace<izz::Geometry>(e, renderable);
  }
}

namespace {}  // namespace
void DeferredRenderer::createGBuffer(int width, int height) {
  // Create a framebuffer for the gbuffer (geometry pass).
  glGenFramebuffers(1, &m_gBufferFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);

  // GBuffer: position texture
  Texture* texture = m_textureSystem->registerTexture(m_gPosition);

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

  // GBuffer: albedoTexture
  glGenTextures(1, &m_gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);  // so that all subsequent calls will affect albedoSpec texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

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

void DeferredRenderer::resize(int width, int height) {
  spdlog::info("Deferred Buffers resized");
  m_screenWidth = width;
  m_screenHeight = height;

  glBindTexture(GL_TEXTURE_2D, m_gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

  glBindTexture(GL_TEXTURE_2D, m_gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  //
  //  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
  //    spdlog::info("Deferred renderer: successfully created framebuffer");
  //  } else {
  //    throw std::runtime_error("Deferred renderer: failed creating a valid frame buffer");
  //  }
}

void DeferredRenderer::createScreenSpaceRect() {
  auto rectangle = izz::geo::PrimitiveFactory::MakePlaneXY("ScreenSpaceRect", 2.0, 2.0);
  const auto& meshBuffer = m_meshSystem->createVertexBuffer(rectangle);

  // DeferredLightingPass UBO is required.
  auto& material = m_materialSystem->createMaterial("DeferredLightingPass");
  if (material.uniformBuffers.count("DeferredLighting") == 0) {
    throw std::runtime_error(fmt::format("{}: material '{}' misses required uniform buffer '{}'", ID, material.name, "DeferredLighting"));
  }

  m_lightPassMaterialId = material.getId();
  m_screenSpaceMeshBufferId = meshBuffer.id;

  // lighting pass material contains slots for textures. Now we can populate them.
  TextureSlot& position = material.getTexture("gbuffer_position");
  position.location = glGetUniformLocation(material.programId, "gbuffer_position");
  position.textureId = m_gPosition;

  TextureSlot& normal = material.getTexture("gbuffer_normal");
  normal.location = glGetUniformLocation(material.programId, "gbuffer_normal");
  normal.textureId = m_gNormal;

  TextureSlot& albedoSpec = material.getTexture("gbuffer_albedospec");
  albedoSpec.location = glGetUniformLocation(material.programId, "gbuffer_albedospec");
  albedoSpec.textureId = m_gAlbedoSpec;

  TextureSlot& environment = material.getTexture("environmentMap");
  environment.location = glGetUniformLocation(material.programId, "environmentMap");

  m_gPositionLoc = glGetUniformLocation(material.programId, "gbuffer_position");
  m_gNormalLoc = glGetUniformLocation(material.programId, "gbuffer_normal");
  m_gAlbedoSpecLoc = glGetUniformLocation(material.programId, "gbuffer_albedospec");
  m_environmentMapLoc = glGetUniformLocation(material.programId, "environmentMap");
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
  //      auto name = m_registry.all_of<lsw::izz::Name>(entity) ? m_registry.get<lsw::izz::Name>(entity).name : "Unnamed";
  //      throw std::runtime_error(fmt::format("Failed initializing curve '{}': {}", name, e.what()));
  //    }
  //  }

  // handling meshes
  spdlog::info("Deferred Renderer: Mesh Initialization");
  auto numRenderables = m_registry.view<DeferredRenderable>().size();
  auto numMeshRenderables = m_registry.view<izz::geo::Mesh, DeferredRenderable>().size_hint();

  // find environment map
  //  auto view = m_registry.view<izz::Skybox>();
  //  if (view.size() != 0) {
  //    auto e = view[0];
  //    auto& skybox = m_registry.get<izz::Skybox>(e);
  //    skybox.

  if (numRenderables != numMeshRenderables) {
    spdlog::warn("It seems some renderable objects are missing a mesh: renderables: {} vs renderables w. mesh: {}", numRenderables, numMeshRenderables);
  }

  for (auto [entity, mesh, r] : m_registry.view<izz::geo::Mesh, DeferredRenderable>().each()) {
    onConstruct(m_registry, entity);
  }

  spdlog::info("=== Deferred Renderer Initialization Complete ===");
}

void DeferredRenderer::update(float dt, float time) {}

void DeferredRenderer::render(const entt::registry& registry) {
  izz::gl::checkError("Deferred Renderer: begin render step");
  static GLenum colorAttachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  static int attachmentCount = sizeof(colorAttachments) / sizeof(GLenum);

  glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFbo);
  glDrawBuffers(attachmentCount, colorAttachments);

  glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // enable writing to depth buffer, and also depth testing.
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // reset to filled mode (to prevent wireframe rendering)

  renderGeometryPass(registry);

  // blit the depth buffer from the gbuffer into the default framebuffer's depth buffer.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBufferFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  renderLightingPass();

  izz::gl::checkError("Deferred Renderer: finish render step");
}

void DeferredRenderer::renderGeometryPass(const entt::registry& registry) {
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  auto view = registry.view<const DeferredRenderable, const izz::ecs::Transform>();

  for (entt::entity e : view) {
    try {
      auto deferred = view.get<const DeferredRenderable>(e);
      auto& mat = m_materialSystem->getMaterialById(deferred.materialId);

      auto meshBufferId = deferred.vertexBufferId;
      const auto& mesh = m_meshSystem->getMeshBuffer(meshBufferId);

      m_materialSystem->updateUniformsForEntity(e, mat);

      // materials are up-to-date. Synchronize them with the GPU
      mat.useProgram();
      mat.useTextures();
      mat.pushUniforms();
      m_meshSystem->bindBuffer(mesh);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      // 1. Render data to G-buffer
      if (mesh.primitiveType == GL_TRIANGLES) {
        glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
      } else {
        glDrawArrays(mesh.primitiveType, 0, mesh.drawElementCount);
      }
    } catch (std::exception& exc) {
      std::string msg = "";

      if (registry.all_of<izz::Name>(e)) {
        auto& name = registry.get<izz::Name>(e);
        msg = fmt::format("(e: {}) Rendering entity: {} - {}", static_cast<int>(e), name.name, exc.what());
      } else {
        msg = exc.what();
      }
      throw std::runtime_error(msg);
    }
  }
}

void DeferredRenderer::renderLightingPass() {
  // lighting pass does not need to check depth mask
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  auto& lightPassMaterial = m_materialSystem->getMaterialById(m_lightPassMaterialId);

  lightPassMaterial.useProgram();
  lightPassMaterial.pushUniforms();
  lightPassMaterial.useTextures2();

  spdlog::info("AAA: environmentMap -- Texture Unit 0: bound texture buffer id {}, on location {}", m_environmentMap, m_environmentMapLoc);

  spdlog::info("AAA: albedoSpecMap -- Texture Unit 1: bound texture buffer id {}, on location {}", m_gAlbedoSpec, m_gAlbedoSpecLoc);
  // bind the gbuffer textures to their appropriate texture binding points.
  //  glActiveTexture(GL_TEXTURE0);
  //  glBindTexture(GL_TEXTURE_2D, m_gPosition);
  //  glUniform1i(m_gPositionLoc, 0);
  spdlog::info("AAA: positionMap -- Texture Unit 2: bound texture buffer id {}, on location {}", m_gPosition, m_gPositionLoc);
  //
  //  glActiveTexture(GL_TEXTURE0 + 1);
  //  glBindTexture(GL_TEXTURE_2D, m_gNormal);
  //  glUniform1i(m_gNormalLoc, 1);
  spdlog::info("AAA: normalMap -- Texture Unit 3: bound texture buffer id {}, on location {}", m_gNormal, m_gNormalLoc);
  //
  //  glActiveTexture(GL_TEXTURE0 + 2);
  //  glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
  //  glUniform1i(m_gAlbedoSpecLoc, 2);

  //
  //  glActiveTexture(GL_TEXTURE0 + 3);
  //  glBindTexture(GL_TEXTURE_2D, m_environmentMapLoc);
  //  glUniform1i(m_environmentMapLoc, 3);

  auto& meshBuffer = m_meshSystem->getMeshBuffer(m_screenSpaceMeshBufferId);
  m_meshSystem->bindBuffer(meshBuffer);

  glDrawElements(meshBuffer.primitiveType, meshBuffer.drawElementCount, GL_UNSIGNED_INT, 0);
}

void DeferredRenderer::setClearColor(const glm::vec4 color) {
  m_clearColor = color;
}