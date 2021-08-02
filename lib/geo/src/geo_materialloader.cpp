//
// Created by jlemein on 10-02-21.
//
#include <geo_materialloader.h>

#include <fstream>
#include <geo_ubermaterialdata.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

using namespace lsw::geo;

namespace {
/// @brief Reserved material names in material definition file.
struct ReservedNames {
  static inline const std::string DIFFUSE_TEXTURE = "diffuseTex";
  static inline const std::string SPECULAR_TEXTURE = "specularTex";
  static inline const std::string NORMAL_TEXTURE = "normalTex";
  static inline const std::string ROUGHNESS_TEXTURE = "roughTex";
};

} // namespace

MaterialSystem::MaterialSystem(const std::string &path)
    : m_materialConfigUri{path} {}

void MaterialSystem::readMaterialMappings(json &j) {
  for (auto md : j["material_mapping"]) {
    auto fromMaterial = md["from"].get<std::string>();
    auto toMaterial = md["to"].get<std::string>();

    if (fromMaterial.empty() || toMaterial.empty()) {
      throw std::runtime_error("Failed to read material mappings");
    }
    if (m_materials.count(toMaterial) <= 0) {
      throw std::runtime_error(
          fmt::format("Mapped to material with name '{}' does not occur in "
                      "material definitions",
                      toMaterial));
    }

    m_materialMappings[fromMaterial] = toMaterial;
  }
}

void addTextureToMaterial(const std::string &textureName,
                          const std::string &path, Material &material) {
  if (textureName == ReservedNames::DIFFUSE_TEXTURE) {
    material.diffuseTexturePath = path;
  } else if (textureName == ReservedNames::SPECULAR_TEXTURE) {
    material.specularTexturePath = path;
  } else if (textureName == ReservedNames::NORMAL_TEXTURE) {
    material.normalTexturePath = path;
  } else if (textureName == ReservedNames::ROUGHNESS_TEXTURE) {
    material.roughnessTexturePath = path;
  } else {
    material.texturePaths[textureName] = path;
  }
}

void MaterialSystem::readMaterialDefinitions(nlohmann::json &j) {
  for (const auto &material : j["materials"]) {
    Material m;
    m.name = material["name"].get<std::string>();

    // pass shader info
    try {
      m.vertexShader = material["vertex_shader"].get<std::string>();
      m.geometryShader = material["geometry_shader"].get<std::string>();
      m.fragmentShader = material["fragment_shader"].get<std::string>();
      spdlog::info("{} --> Vertex shader: {}", m.name, m.vertexShader);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(
          fmt::format("Failed parsing shader names: {}", e.what()));
    }

    try {
      // load generic attributes
      auto &properties = material["properties"];
      for (const auto &prop : properties) {
        auto name = prop["name"].get<std::string>();

        try {
          auto type = prop.count("type") <= 0 ? "float"
                                              : prop["type"].get<std::string>();

          if (type == std::string{"texture"}) {
            addTextureToMaterial(name, prop["value"].get<std::string>(), m);
          } else {
            if (prop["value"].is_array()) {
              auto floatArray = prop["value"].get<std::vector<float>>();
              m.setFloatArray(name, floatArray);
            } else {
              auto value = prop["value"].get<float>();
              m.setFloat(name, value);
            }
          }
        } catch (std::runtime_error &e) {
          spdlog::error(
              "Failed to parse property {} for material {}, details: {}", name,
              m.name, e.what());
          throw e;
        }
      }

      //      auto diffuse = properties["diffuse_color"];
      //      if (diffuse.size() >= 3) {
      //        m.diffuse = glm::vec3(diffuse[0].get<float>(),
      //        diffuse[1].get<float>(),
      //                              diffuse[2].get<float>());
      //        m.hasDiffuse = true;
      //      }
      //      auto specular = properties["specular_color"];
      //      if (specular.size() >= 3) {
      //        m.specular =
      //            glm::vec3(specular[0].get<float>(),
      //            specular[1].get<float>(),
      //                      specular[2].get<float>());
      //        m.hasSpecular = true;
      //      }
      //      auto ambient = properties["ambient_color"];
      //      if (ambient.size() >= 3) {
      //        m.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
      //        m.hasAmbient = true;
      //      }

      // TODO: for now we parse properties here
      if (m.name == std::string{"UberMaterial"}) {
        UberMaterialData data = UberMaterialData::FromMaterial(m);
        m.setProperty(data);
      }
      //      else if (m.name == std::string{"DefaultMaterial"}) {
      //
      //      }

      spdlog::info("Material {} with ambient color {}, {}, {}", m.name,
                   m.ambient.r, m.ambient.g, m.ambient.b);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(
          fmt::format("Failed reading standard material color (diffuse, "
                      "specular or ambient): {}",
                      e.what()));
    }

    m_materials[m.name] = m; // std::move(m);
    spdlog::info("Loaded material with name {}",
                 m_materials.at(m.name).vertexShader);
  }
}

void MaterialSystem::initialize() {
  // create an empty structure (null)
  std::ifstream input(m_materialConfigUri);
  if (input.fail()) {
    auto msg =
        fmt::format("MaterialSystem fails to initialize: cannot read from '{}'",
                    m_materialConfigUri);
    spdlog::error(msg);
    throw std::runtime_error(msg);
  }

  json j;
  input >> j;

  // initialization of material system needs to be done in order.
  // first material definitions are read.
  readMaterialDefinitions(j);
  readMaterialMappings(j);

  if (m_materials.count(j["default_material"]) > 0) {
    m_defaultMaterial = j["default_material"];
  } else {
    spdlog::warn("Cannot set default material to {}",
                 j["default_material"].get<std::string>());
  }
}

bool MaterialSystem::isMaterialDefined(const std::string &materialName) {
  return m_materialMappings.count(materialName) > 0 ||
         m_materials.count(materialName) > 0;
}

std::unique_ptr<lsw::res::IResource>
MaterialSystem::createResource(const std::string &reqMaterialName) {
  auto materialName = m_materialMappings.count(reqMaterialName) > 0
                          ? m_materialMappings.at(reqMaterialName)
                          : reqMaterialName;

  Material material;
  if (m_materials.count(materialName) > 0) {
    material = m_materials.at(materialName);
  } else {
    if (m_materials.count(m_defaultMaterial) > 0) {
      spdlog::warn("Material with name '{}' not found, default material used.",
                   materialName);
      material = m_materials.at(m_defaultMaterial);
    } else {
      throw std::runtime_error("Cannot load material. Material does not exist "
                               "and no default material set");
    }
  }

  return std::make_unique<lsw::res::Resource<geo::Material>>(material);
}

std::unique_ptr<lsw::res::IResource> MaterialSystem::makeDefaultMaterial() {
  return createResource(m_defaultMaterial);
}