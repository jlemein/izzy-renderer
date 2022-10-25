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

//struct ModelViewProjection {
//  glm::mat4 model;
//  glm::mat4 view;
//  glm::mat4 proj;
//  glm::vec3 viewPos;
//};

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

}  // namespace glrs
}  // namespace lsw
