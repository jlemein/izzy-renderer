//
// Created by jeffrey on 24-4-22.
//
#include <fmt/format.h>
#include <izzgl_materialreader.h>
#include <izzgl_materialsystem.h>
#include <izzgl_shadersystem.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace izz::gl;

namespace {
template <typename T>
bool isFloatArray(const T& array) {
  return std::all_of(array.begin(), array.end(), [](const nlohmann::json& el) { return el.is_number_float(); });
}

template <typename T>
bool isIntArray(const T& array) {
  return std::all_of(array.begin(), array.end(), [](const nlohmann::json& el) { return el.is_number_integer(); });
}

izz::geo::TextureHint GetTextureHintFromString(std::string s) {
  static std::unordered_map<std::string, izz::geo::TextureHint> hintMap = {
    {"DIFFUSE", izz::geo::TextureHint::DIFFUSE_MAP},
    {"NORMAL", izz::geo::TextureHint::NORMAL_MAP},
    {"ROUGHNESS", izz::geo::TextureHint::ROUGHNESS_MAP},
    {"HEIGHT", izz::geo::TextureHint::HEIGHT_MAP},
    {"SPECULAR", izz::geo::TextureHint::SPECULAR_MAP}
  };
  return hintMap.at(s);
}

}  // namespace

MaterialReader::MaterialReader(std::shared_ptr<MaterialSystem> materialSystem)
  : m_materialSystem{materialSystem} {}

void MaterialReader::readMaterials(std::filesystem::path path) {
  std::ifstream input(path);
  if (input.fail()) {
    throw std::runtime_error(fmt::format("MaterialSystem fails to initialize: cannot read from '{}'", path.c_str()));
  }

  nlohmann::json j;
  input >> j;

  // shader root decides from where relative paths are resolved.
  std::filesystem::path shaderRoot = "/";
  if (path.is_absolute()) {
    shaderRoot = path.parent_path();
  } else {
    shaderRoot = std::filesystem::current_path() / path.parent_path();
  }

  if (j.contains("shader_root")) {
    std::filesystem::path p = j["shader_root"].get<std::string>();
    if (p.is_absolute()) {
      shaderRoot = p;
    } else {
      shaderRoot += j["shader_root"].get<std::string>();
    }
  }
  spdlog::info("Shader root is: {}", shaderRoot.c_str());
  m_materialSystem->setShaderRootDirectory(shaderRoot);

  readMaterialDefinitions(path.parent_path(), j);
  readMaterialInstances(j);

  if (j.contains("default_material")) {
    m_materialSystem->setDefaultMaterial(j["default_material"]);
  }

  input.close();
}

static void readUniformDescription(std::string key, const nlohmann::json& value, izz::geo::MaterialTemplate& materialDescription) {
  if (value.is_boolean()) {
    materialDescription.uniforms[key] = izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::BOOL, .value = value.get<bool>()};
  } else if (value.is_number_float()) {
    materialDescription.uniforms[key] = izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::FLOAT, .value = value.get<float>()};
  } else if (value.is_number_integer()) {
    materialDescription.uniforms[key] = izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::INT, .value = value.get<int>()};
  } else if (value.is_array()) {
    if (isFloatArray<>(value)) {
      auto v = value.get<std::vector<float>>();
      materialDescription.uniforms[key] =
          izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::FLOAT_ARRAY, .value = v, .length = static_cast<int>(v.size())};
    } else if (isIntArray<>(value)) {
      auto v = value.get<std::vector<int32_t>>();
      materialDescription.uniforms[key] =
          izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::INT_ARRAY, .value = v, .length = static_cast<int>(v.size())};
    }
  } else if (value.is_object()) {
    // scoped uniform (i.e. using interface block)

    //            if (m_uniformBlockManagers.count(name) <= 0) {
    //              throw std::runtime_error(fmt::format(
    //                  "{} with uniform block '{}': material system does not contain an UBO handler for this UBO name. Shader will likely misbehave.",
    //                  materialDescription.name, name));
    //            } else {

    materialDescription.uniformBuffers[key] = izz::geo::UniformBufferDescription{.name = key};

    // TODO: one problem of recursive call. Uniform properties are added to a list and treated as unscoped uniforms.
    //  could cause issues with allocated memory.
    for (const auto& [propName, value] : value.items()) {
      // recursive call.
      readUniformDescription(key + "." + propName, value, materialDescription);
    }
  }
}

void MaterialReader::readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& j) {
  spdlog::info("Reading material definitions...");

  // Loop through the materials
  // - First fetch the shader file names and info.
  // - Then read texture information
  // - Then uniform properties.
  for (const auto& materialJson : j["materials"]) {
    izz::geo::MaterialTemplate materialTemplate;
    materialTemplate.name = materialJson["name"].get<std::string>();
    spdlog::debug("MaterialReader: reading material description \"{}\"...", materialTemplate.name);

    // pass shader info
    try {
      materialTemplate.isBinaryShader = materialJson.contains("is_binary_shader") ? materialJson["is_binary_shader"].get<bool>() : false;
      materialTemplate.vertexShader = parent_path / materialJson["vertex_shader"].get<std::string>();
      materialTemplate.geometryShader = parent_path / materialJson["geometry_shader"].get<std::string>();
      materialTemplate.fragmentShader = parent_path / materialJson["fragment_shader"].get<std::string>();

      spdlog::debug("\tvertex shader: {}", materialTemplate.vertexShader);
      spdlog::debug("\tgeometry shader: {}", materialTemplate.geometryShader);
      spdlog::debug("\tfragment shader: {}", materialTemplate.fragmentShader);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(fmt::format("Failed parsing shader names: {}", e.what()));
    }

    readCompileConstants(materialTemplate, materialJson);

    readBlendState(materialTemplate, materialJson);

    readTextures(materialTemplate, materialJson);

    try {
      if (materialJson.contains("properties")) {
        // dealing with uniform props not in an interface block
        for (const auto& [key, value] : materialJson["properties"].items()) {
          readUniformDescription(key, value, materialTemplate);
        }
      }
    } catch (std::runtime_error& e) {
      throw std::runtime_error(fmt::format("Failed to parse material properties for material {}, details: {}", materialTemplate.name, e.what()));
    }

    m_materialSystem->addMaterialTemplate(materialTemplate);
  }

  spdlog::debug("READING MATERIALS [DONE]");
}

void MaterialReader::readCompileConstants(izz::geo::MaterialTemplate& materialTemplate, const nlohmann::json& json) {
  if (json.contains(KEY_COMPILE_CONSTANTS)) {
    if (!json.is_object()) {
      auto msg = fmt::format("Invalid compile time constants defined for material template '{}'", materialTemplate.name);
      throw std::runtime_error(msg);
    }
    for (const auto& [key, value] : json[KEY_COMPILE_CONSTANTS].items()) {
      if (value) {
        materialTemplate.compileConstants.flags.insert(key);
        spdlog::debug("Added flag '{}' to material template '{}'.", key, materialTemplate.name);
      }
    }
  }
}

void MaterialReader::readBlendState(izz::geo::MaterialTemplate& materialTemplate, const nlohmann::json& json) {
  if (json.contains("blendMode")) {
    std::string blendMode = json["blendMode"].get<std::string>();
    if (blendMode == "ALPHA_BLEND") {
      materialTemplate.blendMode = izz::geo::BlendMode::ALPHA_BLEND;
    } else if (blendMode == "OPAQUE") {
      materialTemplate.blendMode = izz::geo::BlendMode::OPAQUE;
    }
  }
}

void MaterialReader::readTextures(izz::geo::MaterialTemplate& materialTemplate, const nlohmann::json& j) {
  // there are two ways of representing textures
  // 1. short way: "property_name": "file_name.png"
  // 2. explicit representation
  if (j.contains("textures")) {
    for (const auto& [key, value] : j["textures"].items()) {
      izz::geo::TextureDescription textureDescription;
      textureDescription.name = key;
      textureDescription.hint = izz::geo::TextureHint::NO_HINT;
      textureDescription.type = izz::geo::PropertyType::TEXTURE_2D;
      textureDescription.path = "";

      try {
        if (value.is_object()) {
          // explicit representation
          if (value.contains("type") && value["type"].get<std::string>() == "CUBE_MAP") {
            textureDescription.type = izz::geo::PropertyType::CUBE_MAP;
          }
          if (value.contains("hint")) {
            textureDescription.hint = GetTextureHintFromString(value["hint"].get<std::string>());
          }
          if (value.contains("path")) {
            textureDescription.path = value["path"].get<std::string>();
          }
        } else {
          // concise representation
          //        try {
          textureDescription.path = value.get<std::string>();
          //        } catch (std::out_of_range&) {
          //          throw std::runtime_error(
          //              fmt::format("Material instance '{}': texture '{}' is not part of material definition '{}'.", instanceName, key, materialTemplateName));
          //        }
        }
      } catch(std::runtime_error& e) {
        throw std::runtime_error(fmt::format("Failed to read texture '{}' for material '{}'", key, materialTemplate.name));
      }
      materialTemplate.textures[key] = textureDescription;
    }
  }
}

void MaterialReader::readMaterialInstances(nlohmann::json& j) {
  for (auto materialInstanceJson : j["material_instances"]) {
    auto instanceName = materialInstanceJson["name"].get<std::string>();
    auto materialTemplateName = materialInstanceJson["material_definition"].get<std::string>();

    if (instanceName.empty() || materialTemplateName.empty()) {
      throw std::runtime_error("Failed to read material instance");
    }

    if (!m_materialSystem->hasMaterialTemplate(materialTemplateName)) {
      throw std::runtime_error(fmt::format("Material instance '{}', instanced from '{}' does not occur in the list of material definitions.", instanceName,
                                           materialTemplateName));
    }

    // make a copy
    auto materialInstance = m_materialSystem->getMaterialTemplate(materialTemplateName);
    materialInstance.name = instanceName;
    m_materialSystem->addMaterialTemplate(materialInstance);

    readCompileConstants(materialInstance, materialInstanceJson);

    readBlendState(materialInstance, materialInstanceJson);

    readTextures(materialInstance, materialInstanceJson);

    if (materialInstanceJson.contains("properties")) {
      for (const auto& [key, value] : materialInstanceJson["properties"].items()) {
        try {
          if (materialInstance.textures.count(key) > 0) {
            auto& texture = materialInstance.textures.at(key);
            texture.path = value.get<std::string>();
          } else {
            auto uniform = materialInstance.uniforms.at(key);
            switch (uniform.type) {
              case izz::geo::PropertyType::TEXTURE_2D:
                uniform.value = value.get<std::string>();
                break;

              case izz::geo::PropertyType::FLOAT4:
                uniform.value = value.get<std::vector<float>>();
                break;

              case izz::geo::PropertyType::FLOAT:
                uniform.value = value.get<float>();
                break;

              case izz::geo::PropertyType::INT:
                uniform.value = value.get<int>();
                break;

              default:
                spdlog::warn("Material property {} is not part of the material definition. Ignored.", key);
                break;
            }
          }
        } catch (std::out_of_range&) {
          throw std::runtime_error(
              fmt::format("Material instance '{}': property '{}' is not part of material definition '{}'.", instanceName, key, materialTemplateName));
        }
      }
    }
    m_materialSystem->addMaterialTemplate(materialInstance);
  }
}
