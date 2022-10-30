//
// Created by jlemein on 07-11-20.
//
#include <izzgl_rendersystem.h>
#include <izzgl_common.cpp>

#include <ecs_camera.h>
#include <ecs_light.h>
#include <geo_mesh.h>
#include <glrs_lightsystem.h>
#include <izz_resourcemanager.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgl_texturesystem.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include "izzgl_shadersystem.h"
using namespace izz;
using namespace izz::gl;

namespace {

int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
  int location = glGetUniformLocation(program, name);
  if (location == -1) {
    throw std::runtime_error(fmt::format("{}: unknown uniform parameter '{}' (material: '{}').", RenderSystem::ID, name, materialName));
  }
  return location;
}

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}
}  // namespace

LightSystem& RenderSystem::getLightSystem() {
  return *m_lightSystem;
}

RenderSystem::RenderSystem(entt::registry& registry, std::shared_ptr<izz::ResourceManager> resourceManager,
                           std::shared_ptr<MaterialSystem> materialSystem,
                           std::shared_ptr<TextureSystem> textureSystem,
                           std::shared_ptr<izz::gl::MeshSystem> meshSystem,
                           std::shared_ptr<izz::gl::LightSystem> lightSystem)
  : m_registry{registry}  //  , m_debugSystem(registry)
  , m_resourceManager{resourceManager}
  , m_materialSystem(materialSystem)
  , m_meshSystem{meshSystem}
  , m_lightSystem{lightSystem}  //  , m_framebuffer{std::make_unique<HdrFramebuffer>()}
  , m_forwardRenderer(materialSystem, textureSystem, meshSystem, registry)
  , m_deferredRenderer(materialSystem, meshSystem, registry) {
  m_materialSystem->setCapabilitySelector(this);
}

void RenderSystem::initPostprocessBuffers() {
  // prepare quad for collecting
  float vertices[] = {
      // pos        // tex
      -1.0f, -1.f, 0.0f, 0.0f, 1.f, 1.f, 1.0f, 1.0f, -1.f, 1.f, 0.0f, 1.0f, -1.f, -1.f, 0.0f, 0.0f, 1.f, -1.f, 1.0f, 0.0f, 1.f, 1.f, 1.0f, 1.0f};
  // vao 1
  glGenVertexArrays(1, &m_quadVao);
  glGenBuffers(1, &m_quadVbo);
  glBindVertexArray(m_quadVao);
  glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + 4, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(2 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderSystem::init(int width, int height) {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  m_clearColor = glm::vec4(0.15F, 0.15F, 0.25F, 1.0F);
  glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);

  GLuint m_gPosition;
  glGenTextures(1, &m_gPosition);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

  m_lightSystem->initialize();

  // onBeginFrame postprocessing screen quad
  initPostprocessBuffers();

  m_forwardRenderer.setClearColor(m_clearColor);
  m_deferredRenderer.setClearColor(m_clearColor);
  m_forwardRenderer.init(width, height);
  m_deferredRenderer.init(width, height);

  auto numMaterials = m_registry.view<Material>().size();
  auto numLights = m_lightSystem->getActiveLightCount();

  auto numDeferredRenderables = m_registry.view<gl::DeferredRenderable>().size();
  auto numForwardRenderables = m_registry.view<gl::ForwardRenderable>().size();
  spdlog::info("Forward renderables: {}, Deferred renderables: {}", numForwardRenderables, numDeferredRenderables);

  // small summary
  spdlog::info(
      "Render system initialized | "
      "Number of material in use: {} | "
      "Number of active lights: {}",
      numMaterials, numLights);
  return;
}

void RenderSystem::onGeometryAdded(izz::SceneGraphEntity& e, izz::gl::RenderStrategy renderStrategy) {
  if (renderStrategy == RenderStrategy::UNDEFINED || renderStrategy == RenderStrategy::FORWARD) {
    m_forwardRenderer.onEntityCreate(e);
    return;
  }

  if (renderStrategy == RenderStrategy::DEFERRED) {
    m_deferredRenderer.onEntityCreate(e);
    return;
  }

  spdlog::error("Specified render strategy '{}' is not supported", renderStrategy);
}

void RenderSystem::update(float dt, float time) {
  // update light visualizations, recompute hierarchies for efficient rendering.
  m_lightSystem->updateLightProperties();

//  m_forwardRenderer.update(dt, time);
  m_deferredRenderer.update(dt, time);

  // updates the materials per frame (once)
  // then updates all materials individually (is this needed)?
  m_materialSystem->update(dt, time);
}

void RenderSystem::render() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_deferredRenderer.render(m_registry);

  // forward renderer should be after deferred render, because forward may render transparency.
  m_forwardRenderer.render(m_registry);
}

void RenderSystem::resize(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

//  m_forwardRenderer.resize(width, height);
  m_deferredRenderer.resize(width, height);
}

RenderCapabilities RenderSystem::selectRenderCapabilities(const MaterialTemplate& materialTemplate) const {
  RenderCapabilities capabilities;

  if (materialTemplate.blendMode == BlendMode::ALPHA_BLEND) {
    capabilities.flags.insert("DEPTHPEELING");
  }

  return capabilities;
}
