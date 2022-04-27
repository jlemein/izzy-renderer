//
// Created by jeffrey on 26-4-22.
//
#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace izz {
namespace geo {

struct UniformBufferDescription {
  std::string name;
};

enum class PropertyType {
  TEXTURE2D,
  CUBEMAP,
  FLOAT_ARRAY,
  FLOAT,
  FLOAT4,
  FLOAT3,
  INT,
  BOOL,  // needed?
  UNIFORM_BUFFER_OBJECT
};

/**
 * @brief Optional hint for a texture. Sometimes when loading a texture, the use-case is already known.
 * Rather than treating it as one of the textures, a hint can be provided so that the renderer can map them
 * automatically to the right texture slot.
 */
enum class TextureHint { NO_HINT = 0, DIFFUSE_TEXTURE, NORMAL_TEXTURE, SPECULAR_TEXTURE, ROUGHNESS_TEXTURE };

struct TextureDescription {
  PropertyType type;           /// @brief type of texture. Intended to be set to any value of TEXTURE_*.
  std::string name;            /// @brief name of uniform attribute in shader (the texture sampler).
  std::filesystem::path path;  /// @brief path to the texture file (if known). Leave empty to indicate no texture need to be loaded, either because it will
                               /// be set later or because it will get assigned a generated texture from previous render passes.
  TextureHint hint{TextureHint::NO_HINT};  /// @brief (Optional) Use-case hint for the texture.
};

using PropertyValue = std::variant<bool, int, float, std::string, std::vector<float>>;

struct UniformDescription {
  std::string name;
  PropertyType type;
  PropertyValue value;
  int length {1};
};

struct MaterialDescription {
  //  std::string name;               /// @brief Name used for instantiating a material via MaterialSystem::createMaterial()
  bool isBinaryShader{false};      /// @brief Are the shader files in binary format (e.g. pre-compiled SPIRV format)?
  std::string vertexShader{""};    /// @brief Path to the vertex GLSL shader file.
  std::string geometryShader{""};  /// @brief Path to the vertex GLSL shader file.
  std::string fragmentShader{""};  /// @brief Path to the vertex GLSL shader file.

  // fallback values
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;
  glm::vec4 ambientColor;

  std::unordered_map<std::string, TextureDescription> textures;

  //  std::unordered_map<std::string, PropertyType> uniforms;

  /// @brief This map contains information about individual uniform parameters (e.g. their data types and default values).
  /// @details Note that all uniform parameters, both part of a scoped/named uniform block, as well as global or unscoped uniform parameters are added
  /// together in this map.
  std::unordered_map<std::string, UniformDescription> uniforms;

  /// @brief This map contains information about the named uniform buffer objects in the shader (such as the size and data pointers).
  std::unordered_map<std::string, UniformBufferDescription> uniformBuffers;
};

//struct MaterialInstanceDescription {
//  const MaterialDescription* pMaterialDescription {nullptr};
//
//  // fallback values
//  glm::vec4 diffuseColor;
//  glm::vec4 specularColor;
//  glm::vec4 ambientColor;
//
//  std::unordered_map<std::string, TextureDescription> textures {};
//
//  /// @brief This map contains information about individual uniform parameters (e.g. their data types and default values).
//  /// @details Note that all uniform parameters, both part of a scoped/named uniform block, as well as global or unscoped uniform parameters are added
//  /// together in this map.
//  std::unordered_map<std::string, UniformDescription> uniforms {};
//
//  /// @brief This map contains information about the named uniform buffer objects in the shader (such as the size and data pointers).
//  std::unordered_map<std::string, UniformBufferDescription> uniformBuffers {};
//};

}  // namespace geo
}  // namespace izz