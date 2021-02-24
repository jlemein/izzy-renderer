//
// Created by jlemein on 04-02-21.
//

#ifndef RENDERER_ECS_SHADER_H
#define RENDERER_ECS_SHADER_H

#include <cstring>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <entt/fwd.hpp>
#include <ecs_texture.h>

namespace affx {
namespace ecs {

struct UberMaterialData {
  glm::vec4 diffuse{1.0F, 0.0F, 0.0F, 0.0F};
  glm::vec4 specular;
  glm::vec4 ambient;
  bool hasDiffuseTex {false};

  static inline const char *PARAM_NAME = "UberMaterial";
};

struct UniformBlockData {
  void *data;
  std::size_t size;
};

/**!
 * @brief A Shader contains all material properties needed to perform a
 * succesful render pass of, either a mesh, curve or postprocess effect.
 */
struct Shader { // alias single RenderPass
  // TODO copying Shaders should be forbidden or fixed.
  using UniformProperties = std::unordered_map<std::string, UniformBlockData>;

//  UniformProperties properties;

  // a shader is possibly dependent on offscreen render buffers (or file textures).
  std::vector<ecs::Texture> textures {};

//  template <typename T> void setProperty(const T &data) {
//    setProperty(T::PARAM_NAME, data);
//  }
//
//  template <typename T> void setProperty(const char *name, const T &data) {
//    if (properties.count(name) > 0) {
//      memcpy(properties.at(name).data, &data, sizeof(T));
//
//    } else {
//      //      properties[name].data = std::unique_ptr<T>{new T,
//      //      std::default_delete<T>()};
//      properties[name].data = new T();
//      memcpy(properties[name].data, &data, sizeof(T));
//    }
//    properties.at(name).size = sizeof(T);
//  }
//
//  void *getProperty(const char *name) {
//    try {
//      return properties.at(name).data;
//    } catch (std::out_of_range &) {
//      std::cerr << "Cannot find shader property " << name << " in shader.";
//      throw std::runtime_error("Cannot find shader property");
//    }
//  }
//
//  template <typename T> T *getProperty() {
//    return reinterpret_cast<T *>(getProperty(T::PARAM_NAME));
//  }
};

} // namespace ecs
} // namespace affx

#endif // RENDERER_ECS_SHADER_H
