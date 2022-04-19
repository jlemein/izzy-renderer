//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <GL/glew.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>
#include "izz.h"
#include <izzgl_texture.h>

namespace izz {
namespace gl {

/**
 * A mapping of data location and size to keep the uniform blocks registered.
 */
struct UniformBlockInfo {
  std::string name;
  void* data;
  std::size_t size;
};

struct LightingInfo {
  // name of UBO struct in shader
  std::string ubo_struct_name;
};

struct UserProperties {
  // not sure if this is correct. ubo_name is overwritten for every new UBO
  std::string ubo_name;  // name of the uniform block in the shader, used by render engine to find ID.

  std::unordered_map<std::string, float> floatValues;
  std::unordered_map<std::string, int> intValues;
  std::unordered_map<std::string, std::vector<float>> floatArrayValues;
  std::unordered_map<std::string, bool> booleanValues;

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
    floatValues[name] = value;
  }

  void setFloatArray(const std::string& name, std::vector<float> floatArray) {
    floatArrayValues[name] = floatArray;
  }

//  void setMatrix(const std::string& name, glm::mat4 matrix) {
//    matrixValues[name] = matrix;
//  }
//
//  const glm::mat4& getMatrix(const std::string& name) {
//    return matrixValues.at(name);
//  }

  void setValue(const std::string& name, const glm::vec4& value) {
    setFloatArray(name, std::vector<float>{value.r, value.g, value.b, value.a});
  }

  void setInt(const std::string& name, int value) {
    intValues[name] = value;
  }

  bool getBoolean(const std::string& name) {
    return booleanValues.at(name);
  }

  void setBoolean(const std::string& name, bool value) {
    booleanValues[name] = value;
  }
};

enum class ColorBuffer { UNUSED, CUBEMAP, BUFFER_1D, BUFFER_2D, TEXTURE_2D, TEXTURE_2D_MULTISAMPLE };

enum class FramebufferFormat { UNUSED, RGBA_FLOAT32, RGBA_UINT8 };
class FramebufferConfiguration {
 public:
  FramebufferFormat colorBuffers[4] = {FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED};
  FramebufferFormat depthBuffer;
  FramebufferFormat outColorBuffers[4] = {FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED, FramebufferFormat::UNUSED};
};

enum class PropertyType {
  TEXTURE2D,
  CUBEMAP,
  FLOAT,
  FLOAT4,
  FLOAT3,
  INT,
  UNIFORM_BUFFER_OBJECT
};

class MaterialProperty {
 public:
  PropertyType type;
  std::string name;
  void* data;

  void* operator=(void* data) {
    return nullptr;
  }
};

/// @brief Holds texture property
struct TextureProperty {
  /// @brief location as obtained via glGetUniformLocation()
  GLint location;

  /// @brief location as obtained via glGenTextures()
  GLint textureBufferId {-1};
};

struct Material {
  MaterialId id {-1};

  /// @brief Program id as obtained via glCreateProgram()
  int programId {0};

  std::string name;

//  MaterialProperty& operator[](const std::string& name) {
//    return textures[name];
//  }

  void setTexture(const std::string& name, Texture* pTexture) {
    if (!textures.contains(name)) {
      auto location = glGetUniformLocation(programId, name.c_str());
      textures[name] = TextureProperty{location, pTexture->bufferId};
    } else {
      textures.at(name).textureBufferId = pTexture->bufferId;
    }
  }

  GLint getTextureBuffer(const std::string& key) const {
    if (textures.count(key) > 0) {
      return textures.at(key).textureBufferId;
    } else {
      return -1;
      //      throw std::runtime_error(fmt::format("Property {} (texture) does not exist for material {}", key, name));
    }
  }

  /// @brief Specifies input and output buffers for the shader.
  /// Materials can be chained to create multi-pass effects.
  FramebufferConfiguration framebufferConfiguration;

  /// Framebuffer object id
  unsigned int fbo {0U};

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
  izz::TextureId diffuseTexture {-1};
  izz::TextureId specularTexture {-1};
  izz::TextureId normalTexture {-1};
  izz::TextureId roughnessTexture {-1};
  izz::TextureId opacityTexture {-1};

  UserProperties unscopedUniforms;  /// @brief uniforms not in an interface block (less efficient).
  UserProperties userProperties;    /// @brief uniforms as part of a interface block (i.e. named uniform buffer object).

  // contains map from name to property type
  std::unordered_map<std::string, PropertyType> propertyTypes;

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

  std::unordered_map<std::string, TextureProperty> textures{};

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



  // geo::ShadingMode shadingMode


  using UniformBlockRegistry = std::unordered_map<std::string, UniformBlockInfo>;

  UniformBlockRegistry uniformBlocks;

  template <typename T>
  void setProperty(const T& data) {
    setProperty(T::PARAM_NAME, data);
  }

  // TODO: deprecated, remove it in favor of the register call below.
  template <typename T>
  void setProperty(const char* name, const T& data) {
    if (uniformBlocks.count(name) > 0) {
      memcpy(uniformBlocks.at(name).data, &data, sizeof(T));

    } else {
      //      properties[name].data = std::unique_ptr<T>{new T,
      //      std::default_delete<T>()};
      uniformBlocks[name].data = new T();
      memcpy(uniformBlocks[name].data, &data, sizeof(T));
    }
    uniformBlocks.at(name).size = sizeof(T);
  }

  void registerUniformBlock(const char* name, void* pData, std::size_t size) {
    if (uniformBlocks.count(name) > 0) {
      throw std::runtime_error("Cannot add a uniform block {} to material {} that already exists.");
      //      memcpy(properties.at(name).data, pData, size);
    } else {
      uniformBlocks[name].data = pData;
      uniformBlocks[name].size = size;
      //      memcpy(properties[name].data, pData, size);
    }
    uniformBlocks.at(name).size = size;
  }

  void* getProperty(const char* name) {
    try {
      return uniformBlocks.at(name).data;
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
