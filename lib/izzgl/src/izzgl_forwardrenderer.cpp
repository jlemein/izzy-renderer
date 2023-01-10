//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <ecs_camera.h>
#include <ecs_transform.h>
#include <ecs_wireframe.h>
#include <geo_mesh.h>
#include <gl_renderable.h>
#include <izz_skybox.h>
#include <izzgl_error.h>
#include <izzgl_forwardrenderer.h>
#include <izzgl_materialsystem.h>
#include <izzgl_meshsystem.h>
#include "izzgl_gpu.h"
using namespace izz::gl;

ForwardRenderer::ForwardRenderer(std::shared_ptr<Gpu> gpu, entt::registry& registry)
  : m_registry{registry}
  , m_gpu{gpu}
  , m_depthPeeling{m_gpu, registry} {
  m_registry.on_construct<gl::ForwardRenderable>().connect<&ForwardRenderer::onConstruct>(this);
}

void ForwardRenderer::onEntityCreate(SceneGraphEntity& e) {
  auto& renderable = e.get<izz::Geometry>();

  const auto& material = m_gpu->materials.getMaterialById(renderable.materialId);
  e.add(ForwardRenderable{.materialId = renderable.materialId, .vertexBufferId = renderable.vertexBufferId, .blendMode = material.blendMode});
}

void ForwardRenderer::onConstruct(entt::registry& registry, entt::entity e) {
  // if forward renderable entity has no renderable component, then it will be added.
  // we need the renderable for the common rendering functionalities (such as mvp updates).
  if (!m_registry.all_of<izz::Geometry>(e)) {
    auto name = m_registry.try_get<izz::Name>(e);
    spdlog::info("ForwardRenderable component added to entity '{}'. Adding Renderable.", name ? name->name : "");

    auto forwardRenderable = m_registry.get<gl::ForwardRenderable>(e);
    auto renderable = izz::Geometry{.materialId = forwardRenderable.materialId, .vertexBufferId = forwardRenderable.vertexBufferId};
    m_registry.emplace<izz::Geometry>(e, renderable);
  }
}

void ForwardRenderer::init(int width, int height) {
  izz::gl::checkError("Forward Renderer: init() begin");
  m_width = width;
  m_height = height;

  m_depthPeeling.init(width, height);

  izz::gl::checkError("Forward Renderer: init() end");
}

void ForwardRenderer::update(float dt, float time) {
  // do nothing - in future the per frame
}

void ForwardRenderer::render(const entt::registry& registry) {
  // reset values - other render strategies might have changed these settings.
  glViewport(0, 0, m_width, m_height);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  // blit the results from deferred renderer (opaque objects) into the opaqueFbo.
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // should already be bound before starting.

  renderOpaqueObjects(registry);
  if (m_isAlphaBlendingEnabled) {
    m_depthPeeling.render();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  izz::gl::checkError("Forward renderer");
}

void ForwardRenderer::renderOpaqueObjects(const entt::registry& registry) {
  glEnable(GL_CULL_FACE);

  const auto view = registry.view<const ForwardRenderable, const izz::ecs::Transform>();

  for (const auto& [e, forward, transform] : view.each()) {
    if (forward.blendMode == izz::BlendMode::OPAQUE) {
      IZZ_STAT_COUNT(OPAQUE_OBJECTS)

      auto& mat = m_gpu->materials.getMaterialById(forward.materialId);
      const auto& mesh = m_gpu->meshes.getMeshBuffer(forward.vertexBufferId);

      m_gpu->materials.updateUniformsForEntity(e, mat);

      mat.useProgram();
      mat.useTextures();
      mat.pushUniforms();
      m_gpu->meshes.bindBuffer(mesh);

      bool isWireframe = forward.isWireframe || registry.any_of<izz::ecs::Wireframe>(e);
      glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);

      if (mesh.primitiveType == GL_TRIANGLES) {
        glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
      } else {
        glDrawArrays(mesh.primitiveType, 0, mesh.drawElementCount);
      }

      izz::gl::checkError("END render opaque objects");
    }
  }
}

void ForwardRenderer::setClearColor(const glm::vec4& color) {
  m_clearColor = color;
}
