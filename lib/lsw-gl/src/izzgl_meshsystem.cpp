//
// Created by jeffrey on 27-4-22.
//
#include <GL/glew.h>
#include <geo_mesh.h>
#include <izzgl_meshsystem.h>
#include <izzgl_meshbuffer.h>

using namespace izz::gl;

namespace {
constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}
}

MeshBuffer& MeshSystem::createMeshBuffer(const lsw::geo::Mesh& mesh) {
  MeshBuffer meshBuffer;
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint tangentSize = mesh.tangents.size() * sizeof(float);
  GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;
  int vertexAttribCount = 0;

  glGenBuffers(1, &meshBuffer.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);  // allocate buffer data only

  if (vertexSize > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, mesh.vertices.data());  // fill partial data - vertices
    ++vertexAttribCount;
  }
  if (normalSize > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize, mesh.normals.data());  // fill partial data - normals
    ++vertexAttribCount;
  }
  if (uvSize > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize, mesh.uvs.data());  // fill partial data - normals
    ++vertexAttribCount;
  }
  if (tangentSize > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize, tangentSize, mesh.tangents.data());  // fill partial data - normals
    ++vertexAttribCount;
  }
  if (bitangentSize > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize + tangentSize, bitangentSize, mesh.bitangents.data());  // fill partial data - normals
    ++vertexAttribCount;
  }

  glGenBuffers(1, &meshBuffer.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

  meshBuffer.vertexAttribCount = vertexAttribCount;
  // position data
  meshBuffer.vertexAttribArray[0].size = 3U;
  meshBuffer.vertexAttribArray[0].buffer_offset = 0U;
  // normal data
  meshBuffer.vertexAttribArray[1].size = 3U;
  meshBuffer.vertexAttribArray[1].buffer_offset = vertexSize;
  // uv data
  meshBuffer.vertexAttribArray[2].size = 2U;
  meshBuffer.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
  // tangent data
  meshBuffer.vertexAttribArray[3].size = 3U;
  meshBuffer.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
  // bitangent data
  meshBuffer.vertexAttribArray[4].size = 3U;
  meshBuffer.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;

  meshBuffer.drawElementCount = mesh.indices.size();
  meshBuffer.primitiveType = GL_TRIANGLES;

  glGenVertexArrays(1, &meshBuffer.vertex_array_object);
  glBindVertexArray(meshBuffer.vertex_array_object);
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

  meshBuffer.id = static_cast<MeshBufferId>(m_allocatedBuffers.size()+1);
  m_allocatedBuffers[meshBuffer.id] = meshBuffer;
  return m_allocatedBuffers.at(meshBuffer.id);
}

void MeshSystem::bindBuffer(const MeshBuffer& meshBuffer) {
  // bind the vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.index_buffer);
  //    glBindVertexArray(renderable.vertex_array_object);

  for (unsigned int i = 0U; i < meshBuffer.vertexAttribCount; ++i) {
    const VertexAttribArray& attrib = meshBuffer.vertexAttribArray[i];
    // todo: use VAOs
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
  }
}

void MeshSystem::bindBuffer(MeshBufferId meshBufferId) {
  bindBuffer(m_allocatedBuffers.at(meshBufferId));
}