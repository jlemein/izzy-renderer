//
// Created by jlemein on 18-02-22.
//
#include <gl_deferredrenderablefactory.h>
#include <gl_deferredrenderer.h>
#include <gl_renderutils.h>
#include <gl_rendersystem.h>
using namespace izz::gl;

DeferredRenderable DeferredRenderableFactory::CreateRenderable(const lsw::geo::Material& material, RenderSystem& renderSystem) {
  gl::DeferredRenderable renderable;
  auto& rs = renderSystem.createRenderState();
  renderable.renderStateId = rs.id;
//  auto material = materialSystem->createMaterial("table_cloth");
  RenderUtils::LoadMaterial(material, rs);

  // MVP
  renderable.mvp = RenderUtils::GetUniformBufferLocation(rs, "UniformBufferBlock");
  renderable.lights = RenderUtils::GetUniformBufferLocation(rs, "ForwardLighting");

//  renderable.materialId = tableCloth.id;
  return renderable;
}
