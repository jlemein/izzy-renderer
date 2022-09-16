//
// Created by jeffrey on 27-4-22.
//
#include <izzgl_meshbuffer.h>
using namespace izz::gl;

namespace {
constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}
}  // namespace

void MeshBuffer::bindBuffer() {
  // bind the vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  //    glBindVertexArray(renderable.vertex_array_object);

  for (unsigned int i = 0U; i < vertexAttribCount; ++i) {
    const VertexAttribArray& attrib = vertexAttribArray[i];
    // todo: use VAOs
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
  }
}