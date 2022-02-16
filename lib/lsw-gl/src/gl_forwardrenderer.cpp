//
// Created by jeffrey on 19-01-22.
//
#include <gl_forwardrenderer.h>
#include <entt/entt.hpp>
#include <GL/glew.h>
#include <gl_renderable.h>
#include <ecs_transform.h>
#include <ecs_wireframe.h>
#include <gl_rendersystem.h>
#include <gl_common.h>
#include <gl_renderutils.h>
using namespace izz::gl;

ForwardRenderer::ForwardRenderer(RenderSystem& renderSystem)
    : m_renderSystem {renderSystem} {}

void ForwardRenderer::render(const entt::registry& registry) {
  // perform deferred rendering first
  const auto view = registry.view</*const ForwardRenderable,*/const Renderable, const lsw::ecs::Transform>();
  for (const auto& [e, renderable, transform] : view.each()) {
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);

//    const auto& renderable = registry.get<const Renderable>(e);

    // for now assume a renderable has one material
    //    auto effect = m_registry.get<izz::geo::Effect>();

    glUseProgram(renderable.renderState.program);

    RenderUtils::ActivateTextures(renderable.renderState);

//    const auto& r = registry.get<const Renderable>(e);

    // TODO: disable in release
    if (renderable.isWireframe || registry.any_of<lsw::ecs::Wireframe>(e)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    RenderUtils::UseBufferedMeshData(renderable.renderState.meshData);
//    // bind the vertex buffers
//    glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);
//    //    glBindVertexArray(renderable.vertex_array_object);
//
//    for (unsigned int i = 0U; i < renderable.vertexAttribCount; ++i) {
//      const VertexAttribArray& attrib = renderable.vertexAttribArray[i];
//      // todo: use VAOs
//      glEnableVertexAttribArray(i);
//      glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
//    }

    // TODO: check if shader is dirty
    //  reason: if we push properties every frame (Except for MVP), we might
    //  unnecessary spend time doing that while we can immediately just render.
//    m_renderSystem.pushShaderProperties(renderable.renderState);
//    if (renderable.renderState.isMvpSupported) {
//      m_renderSystem.pushModelViewProjection(renderable.renderState);
//    }

    // TODO this one needs to change per glUseProgram, which is the case right
    //  now. In future we might optimize changing of glUseProgram in that
    //  case, this function should be called once per set of glUseProgram.
//    if (renderable.renderState.isLightingSupported) {
//      m_renderSystem.pushLightingData(renderable.renderState);
//    }
    RenderUtils::ActivateUniformProperties(renderable.renderState);

    if (renderable.renderState.meshData.primitiveType == GL_TRIANGLES) {
      glDrawElements(renderable.renderState.meshData.primitiveType, renderable.renderState.meshData.drawElementCount, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(renderable.renderState.meshData.primitiveType, 0, renderable.renderState.meshData.drawElementCount);
    }

//    // handle debug
//    checkError(e);

  }
}
