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
#include "geo_capabilities.h"

namespace izz {
namespace geo {

struct UniformBufferDescription {
  std::string name;
};

enum class PropertyType {
  TEXTURE_2D,
  CUBE_MAP,
  FLOAT_ARRAY,
  FLOAT,
  FLOAT4,
  FLOAT3,
  INT_ARRAY,
  INT,
  BOOL,  // needed?
  UNIFORM_BUFFER_OBJECT
};

/**
 * @brief Optional hint for a texture. Sometimes when loading a texture, the use-case is already known.
 * Rather than treating it as one of the textures, a hint can be provided so that the renderer can map them
 * automatically to the right texture slot.
 */
enum class TextureHint { NO_HINT = 0, DIFFUSE_MAP, NORMAL_MAP, SPECULAR_MAP, ROUGHNESS_MAP, HEIGHT_MAP };

struct TextureDescription {
  PropertyType type;           /// @brief type of texture. Intended to be set to any value of TEXTURE_*.
  std::string name;            /// @brief name of uniform attribute in shader (the texture sampler).
  std::filesystem::path path;  /// @brief path to the texture file (if known). Leave empty to indicate no texture need to be loaded, either because it will
                               /// be set later or because it will get assigned a generated texture from previous render passes.
  TextureHint hint{TextureHint::NO_HINT};  /// @brief (Optional) Use-case hint for the texture.
};

using PropertyValue = std::variant<bool, int, float, std::string, std::vector<float>, std::vector<int32_t>>;

struct UniformDescription {
  std::string name;
  PropertyType type;
  PropertyValue value;
  int length{1};
};

enum class BlendMode {
  OPAQUE = 0,   // no blending.
  ALPHA_BLEND,  // use alpha channel to blend.
  ALPHA_CUTOUT  // binary test alpha testing.
};

// enum class TextureType {
//   TEXTURE_2D = 0, // default 2d texture
//   CUBEMAP         // texture used for cube map.
// };
//
// enum class TextureMetaTag { NO_META_TAG = 0,
//   DIFFUSE,
//   NORMAL,
//   ALBEDO,
//   SPECULAR,
//   ROUGHNESS,
//   HEIGHTMAP
// };

/**
 * A material template represents the definition, description or blueprint of a shader object.
 * It therefore has all information to instantiate a shader. This means:
 * - it knows the vertex, fragment shader and potential other shader files.
 * - it knows the definition of the shader's uniform attributes (for uniform blocks and textures).
 * - it might have default values for the uniform attributes.
 *
 * A material template is used to instantiate a concrete @see Material object. When a material
 * gets instantiated, only then the shader files get compiled.
 * It is very similar to how classes and objects work. The template is the class, the material is the object.
 */
struct MaterialTemplate {
  std::string name{"<unnamed>"};
  bool isBinaryShader{false};      /// @brief Are the shader files in binary format (e.g. pre-compiled SPIRV format)?
  std::string vertexShader{""};    /// @brief Path to the vertex GLSL shader file.
  std::string geometryShader{""};  /// @brief Path to the vertex GLSL shader file.
  std::string fragmentShader{""};  /// @brief Path to the vertex GLSL shader file.

  // fallback values
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;
  glm::vec4 ambientColor;

  BlendMode blendMode{BlendMode::OPAQUE};

  std::unordered_map<std::string, TextureDescription> textures;

  Capabilities compileConstants;

  //  std::unordered_map<std::string, PropertyType> uniforms;

  /// @brief This map contains information about individual uniform parameters (e.g. their data types and default values).
  /// @details Note that all uniform parameters, both part of a scoped/named uniform block, as well as global or unscoped uniform parameters are added
  /// together in this map.
  std::unordered_map<std::string, UniformDescription> uniforms;

  /// @brief This map contains information about the named uniform buffer objects in the shader (such as the size and data pointers).
  std::unordered_map<std::string, UniformBufferDescription> uniformBuffers;
};

}  // namespace geo
}  // namespace izz