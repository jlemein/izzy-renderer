//
// Created by jlemein on 08-11-20.
//
#pragma once

#include <GL/glew.h>
#include <geo_effect.h>
#include <glm/glm.hpp>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "ecs_texture.h"
#include "gl_renderutils.h"
#include "izzgl_meshbuffer.h"

namespace izz {
namespace gl {

struct ModelViewProjection {
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

struct Posteffect {
  GLuint program;

  /// @brief contains ID of the vertex buffer; Usually shared for posteffects applied to same
  GLuint vertex_buffer_object;
  GLuint vertex_array_object;
};

struct Renderable {
  // storage of user defined shader properties
  //  unsigned int materialId;
  std::shared_ptr<izz::gl::Material> material;
//  std::shared_ptr<izz::geo::Effect> effect {nullptr};

  // 0 - forward rendered
  // 1 - deferred rendered
  int category {0};

  //  std::string name{"Unnamed"};
//  RenderState renderState;

  // unscoped uniforms

  // this is done to make sure different light structs are supported.
  const void* pUboLightStruct{nullptr};  // address of light struct - maintained by light system
  unsigned int pUboLightStructSize{0U};  // size of the struct

  // TODO: check if we can only include this property in debug mode (for performance reasons)
  // TODO merge with Wireframe component, and/or use wireframe shader.
  bool isWireframe{false};

  std::vector<izz::ecs::Texture> textures;

//  UnscopedUniforms unscopedUniforms;

  ModelViewProjection uniformBlock{glm::mat4(1.0F), glm::mat4(1.0F), glm::mat4(1.0F), glm::vec4(0.0)};
};

}  // namespace glrs
}  // namespace lsw
