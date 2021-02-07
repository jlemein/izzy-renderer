//
// Created by jlemein on 08-11-20.
//

#ifndef GLVIEWER_VIEWER_RENDERABLE_H
#define GLVIEWER_VIEWER_RENDERABLE_H

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <ecs_shader.h>

#include <cstring>
#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>

namespace affx
{
namespace ecs
{
struct UniformBlock {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct ColorBlock {
  glm::vec4 color;
};

struct UniformLighting {
  glm::vec4 positions[4];
  glm::vec4 intensities[4];
  glm::vec4 colors[4];

  /// MAX = 4
  uint32_t numberLights {0U};
  static inline const char* PARAM_NAME = "Lighting";
};

struct VertexAttribArray {
  GLint size;
  GLuint buffer_offset;
};


struct Renderable_UniformBlockInfo {
  GLuint bufferId;
  GLint blockIndex;
  GLint blockBinding;
  const UniformBlockData* pData {nullptr};
};

struct Renderable {
  //storage of user defined shader properties

//  std::string name{"Unnamed"};
  GLuint program;
  GLuint vertex_buffer{0U}, index_buffer{0U};
  GLuint vertex_array_object;

  GLuint vertexAttribCount{0};
  VertexAttribArray vertexAttribArray[8];
  GLenum primitiveType;
  GLuint drawElementCount;

  /* shader specific details */
  GLuint uboId {0};  // id of the buffer object
  GLint uboBlockIndex {-1};  // arbitrary location in the shader (decided upon by GLSL compiler)
  GLint uboBlockBinding {-1}; //

  // stores simple lighting properties
  bool isLightingSupported{false};
  GLuint uboLightingId {0};     // id as returned from glGenBuffers(GL_UNIFORM,...)
  GLint uboLightingIndex {-1};   // index as determined by GLSL compiler
  GLint uboLightingBinding {-1}; // binding as specified in shader (binding = x)

  // TODO: check if we can only include this property in debug mode (for performance reasons)
  bool isWireframe {false};

  std::unordered_map<std::string, Renderable_UniformBlockInfo> userProperties;

  UniformBlock uniformBlock{glm::mat4(1.0F), glm::mat4(1.0F), glm::mat4(1.0F)};
};

} // end package name
} // end enterprise name

#endif // GLVIEWER_VIEWER_RENDERABLE_H
