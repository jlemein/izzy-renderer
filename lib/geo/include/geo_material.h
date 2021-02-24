//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_MATERIAL_H
#define RENDERER_GEO_MATERIAL_H

#include <fmt/format.h>
#include <geo_texture.h>
#include <memory>
#include <res_resource.h>

namespace affx {
namespace geo {

//struct RenderPass {
//  std::string vertexShader {""};
//  std::string geometryShader {""};
//  std::string fragmentShader {""};
//
//  // TODO copying Shaders should be forbidden or fixed.
//  using UniformProperties = std::unordered_map<std::string, UniformBlockData>;
//
//  UniformProperties properties;
//
//  // a shader is possibly dependent on offscreen render buffers (or file textures).
//  std::vector<ecs::Texture> textures {};
//};

struct UniformBlockData {
  void *data;
  std::size_t size;
};

struct Material {
  std::string name;

  std::string vertexShader {""};
  std::string geometryShader {""};
  std::string fragmentShader {""};

  // colors
  glm::vec3 diffuse;
  glm::vec3 emissive;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 transparent;

  std::shared_ptr<res::Resource<geo::Texture>> diffuseTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> specularTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> normalTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> roughnessTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> opacityTexture {nullptr};

  // geo::ShadingMode shadingMode

  // material
  float shininess;
  float opacity;

  using UniformProperties = std::unordered_map<std::string, UniformBlockData>;

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
      throw std::runtime_error(fmt::format("Cannot find shader property {} in shader.", name));
    }
  }

  template <typename T> T *getProperty() {
    return reinterpret_cast<T *>(getProperty(T::PARAM_NAME));
  }
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_MATERIAL_H
