//
// Created by jeffrey on 24-4-22.
//
#include <fmt/format.h>
#include <izzgl_materialreader.h>
#include <izzgl_materialsystem.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace izz::gl;

namespace {
template <typename T>
bool isFloatArray(const T& array) {
  return std::all_of(array.begin(), array.end(), [](const nlohmann::json& el) { return el.is_number_float(); });
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
  } else if (value.is_array() && isFloatArray<>(value)) {
    auto v = value.get<std::vector<float>>();
    materialDescription.uniforms[key] =
        izz::geo::UniformDescription{.name = key, .type = izz::geo::PropertyType::FLOAT_ARRAY, .value = v, .length = static_cast<int>(v.size())};
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
  for (const auto& material : j["materials"]) {
    izz::geo::MaterialTemplate materialDescription;
    materialDescription.name = material["name"].get<std::string>();
    spdlog::debug("MaterialReader: reading material description \"{}\"...", materialDescription.name);

    // pass shader info
    try {
      materialDescription.isBinaryShader = material.contains("is_binary_shader") ? material["is_binary_shader"].get<bool>() : false;
      materialDescription.vertexShader = parent_path / material["vertex_shader"].get<std::string>();
      materialDescription.geometryShader = parent_path / material["geometry_shader"].get<std::string>();
      materialDescription.fragmentShader = parent_path / material["fragment_shader"].get<std::string>();

      spdlog::debug("\tvertex shader: {}", materialDescription.vertexShader);
      spdlog::debug("\tgeometry shader: {}", materialDescription.geometryShader);
      spdlog::debug("\tfragment shader: {}", materialDescription.fragmentShader);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(fmt::format("Failed parsing shader names: {}", e.what()));
    }

    if (material.contains("textures")) {
      for (const auto& [name, value] : material["textures"].items()) {
        if (!value.contains("type")) {
          spdlog::warn("Material '{}': texture misses required attributes 'type'. Property will be ignored.", materialDescription.name);
          continue;
        }

        izz::geo::TextureDescription textureDescription;
        textureDescription.name = name;
        textureDescription.path = value.contains("default_value") ? value["default_value"].get<std::string>() : "";
        textureDescription.type = izz::geo::PropertyType::TEXTURE2D;

        if (value.contains("type")) {
          if (value["type"] == "texture") {
            textureDescription.type = izz::geo::PropertyType::TEXTURE2D;
            spdlog::debug("\t{} = {}", name, textureDescription.path.c_str());
          } else {
            spdlog::warn("\tUnsupported property {} of type {}. Property will be ignored.", name, value["type"]);
          }
        } else {
          spdlog::warn("\tno texture type specified for material {}. Assuming texture is a TEXTURE2D", name);
        }
        materialDescription.textures[name] = textureDescription;
      }
    }

    try {
      if (material.contains("properties")) {
        // dealing with uniform props not in an interface block
        for (const auto& [key, value] : material["properties"].items()) {
          readUniformDescription(key, value, materialDescription);
        }
      }
    } catch (std::runtime_error& e) {
      throw std::runtime_error(fmt::format("Failed to parse material properties for material {}, details: {}", materialDescription.name, e.what()));
    }

    m_materialSystem->addMaterialTemplate(materialDescription);
  }

  spdlog::debug("READING MATERIALS [DONE]");
}

void MaterialReader::readMaterialInstances(nlohmann::json& j) {
  for (auto instance : j["material_instances"]) {
    auto instanceName = instance["name"].get<std::string>();
    auto materialTemplateName = instance["material_definition"].get<std::string>();

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

    if (instance.contains("textures")) {
      for (const auto& [key, value] : instance["textures"].items()) {
        try {
          auto& texture = materialInstance.textures.at(key);
          texture.path = value.get<std::string>();
        } catch (std::out_of_range&) {
          throw std::runtime_error(
              fmt::format("Material instance '{}': texture '{}' is not part of material definition '{}'.", instanceName, key, materialTemplateName));
        }
      }
    }
    if (instance.contains("properties")) {
      for (const auto& [key, value] : instance["properties"].items()) {
        try {
          if (materialInstance.textures.count(key) > 0) {
            auto& texture = materialInstance.textures.at(key);
            texture.path = value.get<std::string>();
          } else {
            auto uniform = materialInstance.uniforms.at(key);
            switch (uniform.type) {
              case izz::geo::PropertyType::TEXTURE2D:
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
