//
// Created by jeffrey on 14-02-22.
//
#include <geo_curve.h>
#include <geo_mesh.h>
#include <gl_renderable.h>
#include <gl_renderutils.h>
#include "izzgl_shadersystem.h"
using namespace izz;
using namespace izz::gl;

namespace {
//void loadUnscopedUniformParameters(const Material& material, RenderState& rs);
int getUniformLocation(GLint program, const char* name, const std::string& materialName);
}  // namespace
constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void RenderUtils::FillBufferedMeshData(const izz::geo::Curve& curve, MeshBuffer& md) {
  glGenBuffers(1, &md.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, curve.vertices.size() * sizeof(float), curve.vertices.data(), GL_STATIC_DRAW);

  md.index_buffer = 0U;
  //  renderable.vertexAttribCount = 1;
  //  renderable.vertexAttribArray[0].size = 3U;
  //  renderable.vertexAttribArray[0].buffer_offset = 0U;
  md.drawElementCount = curve.vertices.size() / 3;
  md.primitiveType = GL_LINES;

  glGenVertexArrays(1, &md.vertex_array_object);
  glBindVertexArray(md.vertex_array_object);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  // disable the buffers to prevent accidental manipulation
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
