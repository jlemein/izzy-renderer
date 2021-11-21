//
// Created by jlemein on 08-11-20.
//

#ifndef GLVIEWER_VIEWER_RENDERABLE_H
#define GLVIEWER_VIEWER_RENDERABLE_H

#include <glm/glm.hpp>
#include <GL/glew.h>

//#include <ecs_shader.h>
#include <geo_material.h>

#include "ecs_texture.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace lsw
{
namespace glrs
{
struct UniformBlock {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec3 viewPos;
};

struct ColorBlock {
  glm::vec4 color;
  bool diffuseShading {false};

  static inline const char* PARAM_NAME = "ColorBlock";
};

struct UniformLight {
    glm::vec4 position;
    glm::vec4 diffuseColor;

};

struct UniformPointLights {
    glm::vec4 positions;
    glm::vec4 diffuse;
    float intensity;
    float attenuation;

    static inline const char* PARAM_NAME = "PointLight";
};

//struct UniformLighting {
//  glm::vec4 positions[4];
//  glm::vec4 diffuseColors[4];
//  float intensities[4];
//  glm::vec4 _padding1[3];
//  float attenuation[4];
//  glm::vec4 _padding2[3];
//
//  /// MAX = 4
//  uint32_t numberLights {0U};
//  static inline const char* PARAM_NAME = "Lighting";
//};

struct VertexAttribArray {
  GLint size;
  GLuint buffer_offset;
};

struct Renderable_UniformBlockInfo {
  GLuint bufferId;
  GLint blockIndex;
  GLint blockBinding;
  const geo::UniformBlockInfo* pData {nullptr};
};

struct Renderable {
  //storage of user defined shader properties
//  unsigned int materialId;
  std::shared_ptr<geo::Material> material;

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

  // this is done to make sure different light structs are supported.
  const void* pUboLightStruct {nullptr}; // address of light struct - maintained by light system
  unsigned int pUboLightStructSize {0U}; // size of the struct

  // TODO: check if we can only include this property in debug mode (for performance reasons)
  // TODO merge with Wireframe component, and/or use wireframe shader.
  bool isWireframe {false};

  std::vector<ecs::Texture> textures;

  std::unordered_map<std::string, Renderable_UniformBlockInfo> userProperties;

  UniformBlock uniformBlock{glm::mat4(1.0F), glm::mat4(1.0F), glm::mat4(1.0F)};
};

} // end package name
} // end enterprise name

#endif // GLVIEWER_VIEWER_RENDERABLE_H
