//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_MATERIAL_H
#define RENDERER_GEO_MATERIAL_H

#include <fmt/format.h>
#include <geo_texture.h>
#include <memory>
#include <res_resource.h>
#include <glm/glm.hpp>

namespace lsw {
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

struct LightingInfo {
  // name of UBO struct in shader
  std::string ubo_struct_name;
};

struct UserProperties {
  std::string ubo_name; // name of the uniform block in the shader, used by render engine to find ID.

  std::unordered_map<std::string, float> floatValues;
  std::unordered_map<std::string, std::vector<float>> floatArrayValues;

  float getFloat(const std::string& key) const {
      if (floatValues.count(key) > 0 ) {
          return floatValues.at(key);
      } else {
          throw std::runtime_error(fmt::format("Property {} (float) does not exist for uniform buffer struct {}", key, ubo_name));
      }
  }

  std::vector<float> getFloatArray(const std::string& key) const {
      if (floatArrayValues.count(key) > 0 ) {
          return floatArrayValues.at(key);
      } else {
          throw std::runtime_error(fmt::format("Property {} (float[]) does not exist for uniform buffer struct {}", key, ubo_name));
      }
  }

  void setFloat(const std::string& name, float value) {
      floatValues[name] = value;
  }

  void setFloatArray(const std::string& name, std::vector<float> floatArray) {
      floatArrayValues[name] = floatArray;
  }

};

struct Material {
  std::string name;

  std::string vertexShader {""};
  std::string geometryShader {""};
  std::string fragmentShader {""};

  LightingInfo lighting;

  std::string shaderLayout {""};

  // special set of attributes
  glm::vec3 diffuse;
  glm::vec3 emissive;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 transparent;

  bool hasDiffuse {false};
  bool hasEmissive {false};
  bool hasSpecular {false};
  bool hasTransparent {false};
  bool hasAmbient {false};

  /// @brief predefined textures that gets mapped to from existing scene files.
  std::shared_ptr<res::Resource<geo::Texture>> diffuseTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> specularTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> normalTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> roughnessTexture {nullptr};
  std::shared_ptr<res::Resource<geo::Texture>> opacityTexture {nullptr};


  UserProperties userProperties;

  std::string diffuseTexturePath {""};
  std::string specularTexturePath {""};
  std::string normalTexturePath {""};
  std::string roughnessTexturePath {""};

  // TODO: move in separate class or struct.
  /// @brief a texture maps from a parameter name (e.g. my_diffuse_tex) to a file path.
  std::unordered_map<std::string, std::string> texturePaths {};
  /// @brief mapping from parameter name to the texture resource.
  std::unordered_map<std::string, std::shared_ptr<res::Resource<geo::Texture>>> textures {};

  void setDiffuseMap(const std::string& path) {
    diffuseTexturePath = path;
  }

  void setNormalMap(const std::string& path) {
    normalTexturePath = path;
  }

  void setRoughnessMap(const std::string& path) {
    roughnessTexturePath = path;
  }

  void setSpecularMap(const std::string& path) {
    specularTexturePath = path;
  }

  void setTexture(const std::string& textureName, std::string filePath) {
    if (filePath != texturePaths[textureName]) {
      texturePaths[textureName] = filePath;
      textures[textureName] = nullptr;
    }
  }

  std::string getTexture(const std::string& key) const {
    if (texturePaths.count(key) > 0 ) {
      return texturePaths.at(key);
    } else {
      return "";
//      throw std::runtime_error(fmt::format("Property {} (texture) does not exist for material {}", key, name));
    }
  }

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
