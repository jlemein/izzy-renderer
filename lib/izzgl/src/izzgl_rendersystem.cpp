//
// Created by jlemein on 07-11-20.
//
#include <izzgl_rendersystem.h>
#include <izzgl_common.cpp>

#include <ecs_camera.h>

#include <geo_mesh.h>
#include <glrs_lightsystem.h>
#include <izzgl_gammacorrectionpe.h>
#include <izzgl_gpu.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgl_postprocessor.h>
#include <izzgl_texturesystem.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include "geo_primitivefactory.h"
#include "izz_skybox.h"
#include "izzgl_error.h"
#include "izzgl_framebufferbuilder.h"
#include "izzgl_shadersystem.h"
#include "izzgl_skysystem.h"
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

RenderSystem::RenderSystem(entt::registry& registry, std::shared_ptr<Gpu> gpu, std::shared_ptr<izz::gl::SkySystem> skySystem,
                           std::shared_ptr<izz::gl::LightSystem> lightSystem)
  : m_registry{registry}
  , m_gpu{gpu}
  , m_skySystem{skySystem}
  , m_lightSystem{lightSystem}  //  , m_framebuffer{std::make_unique<HdrFramebuffer>()}
  , m_forwardRenderer(gpu, registry)
  , m_deferredRenderer(gpu, registry)
  , m_postProcessor(*gpu, nullptr)
  , m_gammaCorrectionPE(gpu, m_postProcessor, registry) {
  m_gpu->materials.setCapabilitySelector(this);
}

void RenderSystem::init(int width, int height) {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  m_clearColor = glm::vec4(0.15F, 0.15F, 0.25F, 0.0F);
  glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);

  GLuint m_gPosition;
  glGenTextures(1, &m_gPosition);
  glBindTexture(GL_TEXTURE_2D, m_gPosition);  // so that all subsequent calls will affect position texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

  //
  auto colorAttachment = m_gpu->textures.allocateTexture(width, height, TextureFormat::RGBA8);
  auto depthAttachment = m_gpu->textures.allocateDepthTexture(width, height);
  auto framebuffer = FramebufferBuilder(*m_gpu).setColorAttachment(colorAttachment).setDepthAttachment(depthAttachment).create();
  m_postProcessor.setFramebuffer(framebuffer);

  m_postProcessor.initialize();
  m_lightSystem->initialize();
  m_skySystem->init(width, height);

  m_gammaCorrectionPE.init(width, height);

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
  m_skySystem->update(dt, time);
  m_forwardRenderer.update(dt, time);
  m_deferredRenderer.update(dt, time);

  m_gammaCorrectionPE.update(dt, time);
  //  // postprocess
  //  auto view = m_registry.view<ecs::Camera>();
  //  if (view.size() > 0) {
  //    auto e = view[0];
  //    if (m_registry.all_of<izz::GammaCorrection>(e)) {
  //      auto& gammaCorrection = m_registry.get<GammaCorrectionPE>(e);
  //      std::cout << "Setting gamma to " << gammaCorrection.gamma << std::endl;
  //      auto& material = m_materialSystem->getMaterialById(m_postEffect);
  //      auto u = material.getUniformBuffer<izz::ufm::GammaCorrection>();
  //      u->invGamma = 1.0/gammaCorrection.gamma;
  //    }
  //  }

  // updates the materials per frame (once)
  // then updates all materials individually (is this needed)?
  m_gpu->materials.update(dt, time);
}

void RenderSystem::render() {
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render skybox first
  m_skySystem->render();

  m_deferredRenderer.render(m_registry);

  // forward renderer should be after deferred render, because forward may render transparency.
  m_forwardRenderer.render(m_registry);

  m_gammaCorrectionPE.render();

  // post effects
}

void RenderSystem::resize(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_skySystem->resize(width, height);
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
