//
// Created by jeffrey on 14-02-22.
//
#include <gl_renderutils.h>
#include <geo_curve.h>
#include <geo_mesh.h>
using namespace izz;
using namespace izz::gl;

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void RenderUtils::FillBufferedMeshData(const lsw::geo::Curve& curve, BufferedMeshData& md) {
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


void RenderUtils::FillBufferedMeshData(const lsw::geo::Mesh& mesh, BufferedMeshData& md) {
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint tangentSize = mesh.tangents.size() * sizeof(float);
  GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;

  glGenBuffers(1, &md.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr,
               GL_STATIC_DRAW);  // allocate buffer data only
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize,
                  mesh.vertices.data());  // fill partial data - vertices
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize,
                  mesh.normals.data());  // fill partial data - normals
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize,
                  mesh.uvs.data());  // fill partial data - normals

  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize, tangentSize,
                  mesh.tangents.data());  // fill partial data - normals

  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize + tangentSize, bitangentSize,
                  mesh.bitangents.data());  // fill partial data - normals

  glGenBuffers(1, &md.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

  md.vertexAttribCount = 5;
  // position data
  md.vertexAttribArray[0].size = 3U;
  md.vertexAttribArray[0].buffer_offset = 0U;
  // normal data
  md.vertexAttribArray[1].size = 3U;
  md.vertexAttribArray[1].buffer_offset = vertexSize;
  // uv data
  md.vertexAttribArray[2].size = 2U;
  md.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
  // tangent data
  md.vertexAttribArray[3].size = 3U;
  md.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
  // bitangent data
  md.vertexAttribArray[4].size = 3U;
  md.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;

  md.drawElementCount = mesh.indices.size();
  md.primitiveType = GL_TRIANGLES;

  glGenVertexArrays(1, &md.vertex_array_object);
  glBindVertexArray(md.vertex_array_object);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize + tangentSize));
  glBindVertexArray(0);
}

void RenderUtils::ActivateTextures(const RenderState& rs) {
  for (int t = 0; t < rs.textureBuffers.size(); ++t) {
    const auto& texture = rs.textureBuffers[t];

    glActiveTexture(GL_TEXTURE0 + t);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glUniform1i(texture.uniformLocation, t);
  }
}

void RenderUtils::UseBufferedMeshData(const BufferedMeshData& md) {
  // bind the vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
  //    glBindVertexArray(renderable.vertex_array_object);

  for (unsigned int i = 0U; i < md.vertexAttribCount; ++i) {
    const VertexAttribArray& attrib = md.vertexAttribArray[i];
    // todo: use VAOs
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
  }
}