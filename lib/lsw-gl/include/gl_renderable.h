//
// Created by jlemein on 08-11-20.
//
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <geo_material.h>
#include <geo_effect.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "ecs_texture.h"

namespace izz {
namespace gl {

struct UniformBlock {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec3 viewPos;
};

struct ColorBlock {
  glm::vec4 color;
  bool diffuseShading{false};

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

struct VertexAttribArray {
  GLint size;
  GLuint buffer_offset;
};

struct Renderable_UniformBlockInfo {
  GLuint bufferId;
  GLint blockIndex;
  GLint blockBinding;
  const lsw::geo::UniformBlockInfo* pData{nullptr};
};

enum class UType : uint32_t { FLOAT, FLOAT2, FLOAT3, FLOAT4, FLOAT_ARRAY, BOOL, INT, INT2, INT3, INT4, INT_ARRAY, MAT2, MAT3, MAT4 };

struct Renderable_UnscopedUniform {
  GLint location;  /// @brief the location of the uniform parameter in the GLSL shader.
  UType type;  /// @brief Type of the uniform property stored. @see UType.
  int offset;  /// @brief Offset in the UnscopedUniforms::pData attribute where the value is stored. Offset is expressed in bytes (uint8_t).
  int length;  /// @brief length of the property. This is expressed in number of elements (i.e. the length of the array irrespective of data type)
};

struct UnscopedUniforms {
//  std::unique_ptr<Renderable_UnscopedUniform[]> pProperties;
  Renderable_UnscopedUniform* pProperties;
  int numProperties;
  //  uint8_t* pData;
  uint8_t* pData;
  std::size_t size;

  static inline UnscopedUniforms Allocate(int numProperties, std::size_t sizeOfData) {
    return UnscopedUniforms{new Renderable_UnscopedUniform[numProperties], numProperties, new uint8_t[sizeOfData], sizeOfData};
  }

  static inline void DeallocateUnscopedUniforms(UnscopedUniforms* unscopedUniforms) {
    delete[] unscopedUniforms->pProperties;
    delete[] unscopedUniforms->pData;
  }
};



struct Posteffect {
  GLuint program;

  /// @brief contains ID of the vertex buffer; Usually shared for posteffects applied to same
  GLuint vertex_buffer_object;
  GLuint vertex_array_object;
};

/// @brief Represents the buffered mesh data, stored on the GPU
struct BufferedMeshData {
  GLuint vertex_buffer{0U}, index_buffer{0U};
  GLuint vertex_array_object;

  GLuint vertexAttribCount{0};
  VertexAttribArray vertexAttribArray[8];
  GLenum primitiveType;
  GLuint drawElementCount;
};

struct TextureBuffer {
  GLuint textureId;  // as obtained via glGenTextures(, &id)
  GLint uniformLocation; // as obtained via glGetUniformLocation
};

struct RenderState {
  int id;
  GLuint program {0};

  BufferedMeshData meshData;
  std::vector<TextureBuffer> textureBuffers;

  // TODO: make one attribute
  std::vector<Renderable_UniformBlockInfo> uniformBlocks;
//  std::unordered_map<std::string, Renderable_UniformBlockInfo> userProperties;
  UnscopedUniforms unscopedUniforms;
//  UniformBlock uniformBlock{glm::mat4(1.0F), glm::mat4(1.0F), glm::mat4(1.0F)};

  /* shader specific details */
//  bool isMvpSupported{false};
//  GLuint uboId{0};            // id of the buffer object
//  GLint uboBlockIndex{-1};    // arbitrary location in the shader (decided upon by GLSL compiler)
//  GLint uboBlockBinding{-1};  //

  // stores simple lighting properties
  bool isLightingSupported{false};
  GLuint uboLightingId{0};       // id as returned from glGenBuffers(GL_UNIFORM,...)
  GLint uboLightingIndex{-1};    // index as determined by GLSL compiler
  GLint uboLightingBinding{-1};  // binding as specified in shader (binding = x)

  const void* pUboLightStruct{nullptr};  // address of light struct - maintained by light system
  unsigned int pUboLightStructSize{0U};  // size of the struct
};

struct Renderable {
  // storage of user defined shader properties
  //  unsigned int materialId;
  std::shared_ptr<lsw::geo::Material> material;
//  std::shared_ptr<izz::geo::Effect> effect {nullptr};

  // 0 - forward rendered
  // 1 - deferred rendered
  int category {0};

  //  std::string name{"Unnamed"};
  RenderState renderState;

  // unscoped uniforms

  // this is done to make sure different light structs are supported.
  const void* pUboLightStruct{nullptr};  // address of light struct - maintained by light system
  unsigned int pUboLightStructSize{0U};  // size of the struct

  // TODO: check if we can only include this property in debug mode (for performance reasons)
  // TODO merge with Wireframe component, and/or use wireframe shader.
  bool isWireframe{false};

  std::vector<lsw::ecs::Texture> textures;

  std::unordered_map<std::string, Renderable_UniformBlockInfo> userProperties;
  UnscopedUniforms unscopedUniforms;

  UniformBlock uniformBlock{glm::mat4(1.0F), glm::mat4(1.0F), glm::mat4(1.0F)};
};

}  // namespace glrs
}  // namespace lsw
