//
// Created by jeffrey on 27-4-22.
//
#pragma once

#include <GL/glew.h>
#include <array>
#include "izz.h"

namespace izz {
namespace gl {

struct VertexAttribArray {
  GLint size;
  GLuint buffer_offset;
};

/// @brief Represents the buffered mesh data, stored on the GPU.
struct MeshBuffer {
  VertexBufferId id;
  GLuint vertex_buffer{0U}, index_buffer{0U};
  GLuint vertex_array_object;

  GLuint vertexAttribCount{0};
  //  VertexAttribArray vertexAttribArray[8];
  std::array<VertexAttribArray, 8> vertexAttribArray;
  GLenum primitiveType;
  GLuint drawElementCount;

  void bindBuffer();
};
}  // namespace gl
}  // namespace izz