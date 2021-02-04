//
// Created by jlemein on 04-02-21.
//

#ifndef RENDERER_ECS_SHADER_H
#define RENDERER_ECS_SHADER_H

#include <cstring>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>

namespace affx {
namespace ecs {

struct UberMaterialData {
  glm::vec3 diffuse{1.0F, 0.0F, 0.0F};
  glm::vec3 specular;
  glm::vec3 ambient;

  static inline const char *PARAM_NAME = "UberMaterial";
};

struct UniformBlockData {
  void *data;
  std::size_t size;
};

struct Shader {
  // TODO copying Shaders should be forbidden or fixed.
  using UniformProperties = std::unordered_map<std::string, UniformBlockData>;

  std::string vertexShaderFile{""};
  std::string fragmentShaderFile{""};
  UniformProperties properties;

  template <typename T> void setProperty(const T &data) {
    setProperty(T::PARAM_NAME, data);
  }

  template <typename T> void setProperty(const char *name, const T &data) {
    if (properties.count(name) > 0) {
      memcpy(properties.at(name).data, &data, sizeof(T));

    } else {
      //      properties[name].data = std::unique_ptr<T>{new T,
      //      std::default_delete<T>()};
      properties[name].data = new T();
      memcpy(properties[name].data, &data, sizeof(T));
    }
    properties.at(name).size = sizeof(T);
  }

  void *getProperty(const char *name) {
    try {
      return properties.at(name).data;
    } catch (std::out_of_range &) {
      std::cerr << "Cannot find shader property " << name << " in shader.";
      throw std::runtime_error("Cannot find shader property");
    }
  }

  template <typename T> T *getProperty() {
    return reinterpret_cast<T *>(getProperty(T::PARAM_NAME));
  }
};

} // namespace ecs
} // namespace affx

#endif // RENDERER_ECS_SHADER_H
