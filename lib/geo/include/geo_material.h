//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <fmt/format.h>
#include <geo_texture.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>

namespace lsw {
namespace geo {

// struct RenderPass {
//   std::string vertexShader {""};
//   std::string geometryShader {""};
//   std::string fragmentShader {""};
//
//   // TODO copying Shaders should be forbidden or fixed.
//   using UniformProperties = std::unordered_map<std::string, UniformBlockData>;
//
//   UniformProperties properties;
//
//   // a shader is possibly dependent on offscreen render buffers (or file textures).
//   std::vector<ecs::Texture> textures {};
// };

/**
 * A mapping of data location and size to keep the uniform blocks registered.
 */
struct UniformBlockInfo {
  void* data;
  std::size_t size;
};

struct LightingInfo {
  // name of UBO struct in shader
  std::string ubo_struct_name;
};

struct UserProperties {
  std::string ubo_name;  // name of the uniform block in the shader, used by render engine to find ID.

  std::unordered_map<std::string, float> floatValues;
  std::unordered_map<std::string, int> intValues;
  std::unordered_map<std::string, std::vector<float>> floatArrayValues;

  float getFloat(const std::string& key) const {
    if (floatValues.count(key) > 0) {
      return floatValues.at(key);
    } else {
      throw std::runtime_error(fmt::format("Property {} (float) does not exist for uniform buffer struct {}", key, ubo_name));
    }
  }

  int getInt(const std::string& key) const {
    if (intValues.count(key) > 0) {
      return intValues.at(key);
    } else {
      throw std::runtime_error(fmt::format("Property {} (int) does not exist for uniform buffer struct {}", key, ubo_name));
    }
  }

  std::vector<float> getFloatArray(const std::string& key) const {
    if (floatArrayValues.count(key) > 0) {
      return floatArrayValues.at(key);
    } else {
      throw std::runtime_error(fmt::format("Property {} (float[]) does not exist for uniform buffer struct {}", key, ubo_name));
    }
  }

  const glm::vec4& getVec4f(const std::string& key) const {
    if (floatArrayValues.count(key) > 0) {
      if (floatArrayValues.at(key).size() < 4) {
        throw std::runtime_error("requesting a vec4f but the requested property is not a vec4f");
      }
      return *reinterpret_cast<const glm::vec4*>(floatArrayValues.at(key).data());
    } else {
      throw std::runtime_error(fmt::format("Property {} (vec4) does not exist for uniform buffer struct {}", key, ubo_name));
    }
  }

  void setFloat(const std::string& name, float value) {
    spdlog::debug("UBO {}: Setting property {} (float) to value: {}", ubo_name, name, value);
    floatValues[name] = value;
  }

  void setFloatArray(const std::string& name, std::vector<float> floatArray) {
    floatArrayValues[name] = floatArray;
  }

  void setValue(const std::string& name, const glm::vec4& value) {
    setFloatArray(name, std::vector<float>{value.r, value.g, value.b, value.a});
  }

  void setInt(const std::string& name, int value) {
    intValues[name] = value;
  }
};

struct Material {
  enum PropertyType {
    TEXTURE2D,
    CUBEMAP,
    FLOAT,
    FLOAT4,
    FLOAT3,
    INT,
    UNIFORM_BUFFER_OBJECT
  };
  std::string name;

  /// Indicates whether the vertex and fragment shader files are in binary format (e.g. pre-compiled SPIRV format).
  bool isBinaryShader{false};

  std::string vertexShader{""};
  std::string geometryShader{""};
  std::string fragmentShader{""};

  LightingInfo lighting;

  std::string shaderLayout{""};

  // special set of attributes
  glm::vec3 diffuse;
  glm::vec3 emissive;
  glm::vec3 ambient;
  glm::vec3 specular;
  glm::vec3 transparent;

  bool hasDiffuse{false};
  bool hasEmissive{false};
  bool hasSpecular{false};
  bool hasTransparent{false};
  bool hasAmbient{false};

  /// @brief predefined textures that gets mapped to from existing scene files.
  std::shared_ptr<geo::Texture> diffuseTexture{nullptr};
  std::shared_ptr<geo::Texture> specularTexture{nullptr};
  std::shared_ptr<geo::Texture> normalTexture{nullptr};
  std::shared_ptr<geo::Texture> roughnessTexture{nullptr};
  std::shared_ptr<geo::Texture> opacityTexture{nullptr};

  UserProperties userProperties;

  // contains map from name to property type
  std::unordered_map<std::string, Material::PropertyType> propertyTypes;

  std::filesystem::path diffuseTexturePath{""};
  std::filesystem::path specularTexturePath{""};
  std::filesystem::path normalTexturePath{""};
  std::filesystem::path roughnessTexturePath{""};

  // TODO: move in separate class or struct.
  /**
   * @brief a texture maps from a parameter name (e.g. my_diffuse_tex) to a file path.
   * When programmatically assigned a texture to an entity, set the texturePath instead of loading the texture in textures.
   */
  std::unordered_map<std::string, std::string> texturePaths{};
  std::unordered_map<std::string, std::string> defaultTexturePaths{};
  /**
   * @brief mapping from parameter name to the texture resource.
   * This variable is for bookkeeping purposes only. Never set this attribute directly, your texture will not be loaded.
   * Use @see texturePaths instead.
   */
  std::unordered_map<std::string, std::shared_ptr<geo::Texture>> textures{};

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
    if (texturePaths.count(key) > 0) {
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

  using UniformBlockRegistry = std::unordered_map<std::string, UniformBlockInfo>;

  UniformBlockRegistry properties;

  template <typename T>
  void setProperty(const T& data) {
    setProperty(T::PARAM_NAME, data);
  }

  // TODO: deprecated, remove it in favor of the register call below.
  template <typename T>
  void setProperty(const char* name, const T& data) {
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

  void registerUniformBlock(const char* name, void* pData, std::size_t size) {
    if (properties.count(name) > 0) {
      throw std::runtime_error("Cannot add a uniform block {} to material {} that already exists.");
      //      memcpy(properties.at(name).data, pData, size);
    } else {
      properties[name].data = pData;
      properties[name].size = size;
      //      memcpy(properties[name].data, pData, size);
    }
    properties.at(name).size = size;
  }

  void* getProperty(const char* name) {
    try {
      return properties.at(name).data;
    } catch (std::out_of_range&) {
      throw std::runtime_error(fmt::format("Cannot find shader property {} in shader.", name));
    }
  }

  template <typename T>
  T* getProperty() {
    return reinterpret_cast<T*>(getProperty(T::PARAM_NAME));
  }
};

}  // namespace geo
}  // namespace lsw
