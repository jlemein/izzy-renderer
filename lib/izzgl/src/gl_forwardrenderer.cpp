//
// Created by jeffrey on 19-01-22.
//
#include <GL/glew.h>
#include <ecs_transform.h>
#include <ecs_wireframe.h>
#include <gl_common.h>
#include <gl_forwardrenderer.h>
#include <gl_renderable.h>
#include "ecs_camera.h"
#include "ecs_name.h"
#include <izzgl_meshsystem.h>
#include <izzgl_materialsystem.h>
using namespace izz::gl;

ForwardRenderer::ForwardRenderer(std::shared_ptr<MaterialSystem> materialSystem, std::shared_ptr<MeshSystem> meshSystem, entt::registry& registry)
  : m_materialSystem{materialSystem}
  , m_meshSystem{meshSystem}
  , m_registry{registry} {
  m_registry.on_construct<gl::ForwardRenderable>().connect<&ForwardRenderer::onConstruct>(this);
}

void ForwardRenderer::onConstruct(entt::registry& registry, entt::entity e) {
  // if deferred entity has no renderable component, then it will be added.
  // we need the renderable for the common rendering functionalities (such as mvp updates).
  if (!m_registry.all_of<gl::Renderable>(e)) {
    auto name = m_registry.try_get<ecs::Name>(e);
    spdlog::info("ForwardRenderable component added to entity '{}'. Adding Renderable.", name ? name->name : "");

    auto forwardRenderable = m_registry.get<gl::ForwardRenderable>(e);
    auto renderable = gl::Renderable{.materialId = forwardRenderable.materialId, .meshBufferId = forwardRenderable.meshBufferId};
    m_registry.emplace<gl::Renderable>(e, renderable);
  }
}

void ForwardRenderer::update(float dt, float time) {
  // do nothing - in future the per frame
}

void ForwardRenderer::render(const entt::registry& registry) {
  // forward renderer uses depth testing and writes into the depth buffer.
  // (other render strategies might have changed these settings).
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  // bind default framebuffer, and set draw buffers to 0 (in case somebody messed with it).
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  const auto view = registry.view<const ForwardRenderable, const izz::ecs::Transform>();

  for (const auto& [e, forward, transform] : view.each()) {
    auto& mat = m_materialSystem->getMaterialById(forward.materialId);
    const auto& mesh = m_meshSystem->getMeshBuffer(forward.meshBufferId);

    m_materialSystem->updateUniformsForEntity(e, mat);

    mat.useProgram();
    mat.useTextures();
    mat.pushUniforms();
    m_meshSystem->bindBuffer(mesh);

    bool isWireframe = forward.isWireframe || registry.any_of<izz::ecs::Wireframe>(e);
    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);

    if (mesh.primitiveType == GL_TRIANGLES) {
      glDrawElements(mesh.primitiveType, mesh.drawElementCount, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(mesh.primitiveType, 0, mesh.drawElementCount);
    }

    izz::gl::checkError("Forward renderer");
  }
}
