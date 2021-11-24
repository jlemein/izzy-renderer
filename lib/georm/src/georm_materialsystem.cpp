//
// Created by jlemein on 05-08-21.
//
#include <georm_materialsystem.h>

#include <ecsg_scenegraph.h>
#include <geo_textureloader.h>
#include <georm_resourcemanager.h>
#include <georm_texturesystem.h>
#include <glrs_rendersystem.h>
#include <uniform_constant.h>
#include <uniform_lambert.h>
#include <uniform_parallax.h>
#include <uniform_ubermaterial.h>
#include <wsp_workspace.h>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <sstream>

using json = nlohmann::json;

using namespace lsw;
using namespace lsw::georm;
using namespace lsw::geo;

namespace {
/// @brief Reserved material names in material definition file.
struct ReservedNames {
  static inline const std::string DIFFUSE_TEXTURE = "diffuseTex";
  static inline const std::string SPECULAR_TEXTURE = "specularTex";
  static inline const std::string NORMAL_TEXTURE = "normalTex";
  static inline const std::string ROUGHNESS_TEXTURE = "roughTex";
};

template <typename T>
bool isFloatArray(const T& array) {
  return std::all_of(array.begin(), array.end(), [](const json& el) { return el.is_number_float(); });
}

}  // namespace

MaterialSystem::MaterialSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<georm::ResourceManager> resourceManager)
  : m_sceneGraph{sceneGraph}
  , m_resourceManager{resourceManager} {
  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBlockManagers[ufm::Lambert::PARAM_NAME] = std::make_unique<ufm::LambertUniformManager>();
  m_uniformBlockManagers[ufm::Parallax::PARAM_NAME] = std::make_unique<ufm::ParallaxManager>();
  m_uniformBlockManagers[ufm::Uber::PARAM_NAME] = std::make_unique<ufm::UberUniformManager>();
  m_uniformBlockManagers[ufm::ConstantLight::PARAM_NAME] = std::make_unique<ufm::ConstantManager>();
}

void MaterialSystem::readMaterialMappings(json& j) {
  for (auto md : j["material_mapping"]) {
    auto fromMaterial = md["from"].get<std::string>();
    auto toMaterial = md["to"].get<std::string>();

    if (fromMaterial.empty() || toMaterial.empty()) {
      throw std::runtime_error("Failed to read material mappings");
    }
    if (m_materials.count(toMaterial) <= 0) {
      throw std::runtime_error(fmt::format("Material mapping '{}' -> '{}' does not occur in the list of materials", fromMaterial, toMaterial));
    }

    m_materialMappings[fromMaterial] = toMaterial;
  }
}

void addTextureToMaterial(const std::string& textureName, const std::string& path, geo::Material& material) {
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

void MaterialSystem::readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& j) {
  spdlog::info("Reading material definitions...");

  for (const auto& material : j["materials"]) {
    Material m;
    m.name = material["name"].get<std::string>();
    spdlog::debug("Reading material with id \"{}\"...", m.name);

    try {
      if (material.count("lighting") > 0) {
        const auto& lighting = material["lighting"];
        m.lighting.ubo_struct_name = lighting["ubo_struct"].get<std::string>();
      } else {
        m.lighting.ubo_struct_name = "";
      }
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(fmt::format("Failed parsing lighting metadata: {}", e.what()));
    }

    // pass shader info
    try {
      m.isBinaryShader = material.contains("is_binary_shader") ? material["is_binary_shader"].get<bool>() : false;
      m.vertexShader = parent_path / material["vertex_shader"].get<std::string>();
      m.geometryShader = parent_path / material["geometry_shader"].get<std::string>();
      m.fragmentShader = parent_path / material["fragment_shader"].get<std::string>();

      spdlog::debug("\tvertex shader: {}", m.vertexShader);
      spdlog::debug("\tgeometry shader: {}", m.geometryShader);
      spdlog::debug("\tfragment shader: {}", m.fragmentShader);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(fmt::format("Failed parsing shader names: {}", e.what()));
    }

    // load generic attributes
    if (material.contains("properties")) {
      auto& properties = material["properties"];
      for (const auto& prop : properties) {
        // Required: every property must have a name and type.
        if (!prop.contains("name") || !prop.contains("type")) {
          spdlog::warn("Material '{}': property misses required attributes 'name' and/or 'type'. Property will be ignored.", m.name);
          continue;
        }

        std::string name = prop["name"];
        std::string type = prop["type"];

        try {
          if (type == "uniform") {
            if (m_uniformBlockManagers.count(name) <= 0) {
              throw std::runtime_error(
                  fmt::format("Material {}: Uniform block '{}' is not registered to the material system. Shader will likely misbehave.", m.name, name));
            } else {
              std::size_t sizeOfStruct = 0;
              auto uniformData = m_uniformBlockManagers.at(name)->CreateUniformBlock(sizeOfStruct);
              m.registerUniformBlock(name.c_str(), uniformData, sizeOfStruct);
              m.userProperties.ubo_name = name;

              auto parameters = prop["value"];

              for (const auto& [key, value] : parameters.items()) {
                if (value.is_array() && isFloatArray<>(value)) {
                  auto list = value.get<std::vector<float>>();
                  spdlog::debug(fmt::format("\t{}::{} = [{}]", name, key, fmt::join(list.begin(), list.end(), ", ")));
                  m.userProperties.setFloatArray(key, value.get<std::vector<float>>());
                } else if (value.is_number_float()) {
                  spdlog::debug("\t{}::{} = {}", name, key, value.get<float>());
                  m.userProperties.setFloat(key, value.get<float>());
                } else if (value.is_number_integer()){
                  spdlog::debug("\t{}::{} = {}", name, key, value.get<int>());
                  m.userProperties.setInt(key, value.get<int>());
                } else {
                  spdlog::error("Material {} with parameter '{}' is ignored. Data type is not supported.", m.name, key);
                }
              }
            }
          } else if (type == "texture") {
            auto path = prop["value"].get<std::string>();
            spdlog::debug("\t{} texture: {}", name, path);
            addTextureToMaterial(name, path, m);
          } else {
            spdlog::warn("Material '{}' defines property {} of type {} that is not supported. Property will be ignored.", m.name, name, type);
          }

        } catch (std::runtime_error& e) {
          spdlog::error("Failed to parse property {} for material {}, details: {}", name, m.name, e.what());
          throw e;
        }
      }
    }

    m_materials[m.name] = m;  // std::move(m);
  }
}

void MaterialSystem::loadMaterialsFromFile(const std::filesystem::path& path) {
  std::ifstream input(path);
  if (input.fail()) {
    throw std::runtime_error(fmt::format("MaterialSystem fails to initialize: cannot read from '{}'", path));
  }

  json j;
  input >> j;

  // initialization of material system needs to be done in order.
  // first material definitions are read.
  readMaterialDefinitions(path.parent_path(), j);
  readMaterialMappings(j);

  if (j.contains("default_material")) {
    auto defaultMaterial = j["default_material"];
    if (m_materials.count(defaultMaterial) > 0) {
      m_defaultMaterial = defaultMaterial;
    } else {
      spdlog::warn("{} specifies a default material '{}' that is not defined in the list of materials", path, defaultMaterial);
    }
  }
}

bool MaterialSystem::isMaterialDefined(const std::string& materialName) {
  return m_materialMappings.count(materialName) > 0 || m_materials.count(materialName) > 0;
}

std::shared_ptr<geo::Material> MaterialSystem::createMaterial(const std::string& name) {
  // 1. First process material mapping to material name
  auto materialName = m_materialMappings.count(name) > 0 ? m_materialMappings.at(name) : name;

  // 2. Match the specified name with a known material
  Material material;
  if (m_materials.count(materialName) > 0) {
    material = m_materials.at(materialName);
  } else {
    if (m_materials.count(m_defaultMaterial) > 0) {
      // 2. If material does not occur, assign a default one (if available)
      spdlog::warn("Material with name '{}' not found, default material used.", materialName);
      material = m_materials.at(m_defaultMaterial);

    } else if (!m_materials.empty()) {
      // 3. Otherwise choose the first one
      material = m_materials.begin()->second;
      spdlog::warn("Material '{}' is not defined. No default material is set. Using the first defined material (0: '{}') instead.", name, material.name);
    } else {
      // 4. Otherwise throw an error
      throw std::runtime_error(fmt::format("Failed to create material '{}': no such material is defined.", materialName));
    }
  }

  return std::make_shared<geo::Material>(material);
}

std::shared_ptr<geo::Material> MaterialSystem::makeDefaultMaterial() {
  return createMaterial(m_defaultMaterial);
}

void MaterialSystem::update(float time, float dt) {
  auto view = m_sceneGraph->getRegistry().view<geo::Material, glrs::Renderable>();
  for (auto e : view) {
    const auto& r = view.get<glrs::Renderable>(e);
    const auto& m = view.get<geo::Material>(e);

    // TODO: probably this for loop is more efficient if it is part of render system
    for (const auto& [name, uniformBlock] : r.userProperties) {
#ifndef NDEBUG
      if (m_uniformBlockManagers.count(name) <= 0) {
        throw std::runtime_error(fmt::format("Material system cannot find manager for ubo block '{}'", name));
      }
#endif  // NDEBUG

      m_uniformBlockManagers.at(name)->UpdateUniform(uniformBlock.pData->data, m);
    }
  }
}

void MaterialSystem::synchronizeTextures(glrs::RenderSystem& renderSystem) {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<geo::Material>();  // why are we not requesting
                                               // <Material, Renderable>?

  // put the code in bottom part of readMaterialDefinitions here
  // ...

  for (auto entity : view) {
    auto& renderable = registry.get_or_emplace<glrs::Renderable>(entity);
    auto& geoMaterial = view.get<geo::Material>(entity);
    spdlog::debug("Attach textures for meterial '{}' to render system", geoMaterial.name);

    for (auto& [name, path] : geoMaterial.texturePaths) {
      // TODO: check if name is used in shader object
      try {
        auto t = m_resourceManager->getTextureSystem()->loadTexture(path);
        if (t != nullptr) {
          geoMaterial.textures[name] = t;
          renderSystem.attachTexture(renderable, *t, name);
        }
      } catch (std::out_of_range e) {
        throw std::runtime_error(fmt::format("Cannot find texture with name \"{}\" in material \"{}\"", name, geoMaterial.name));
      }
    }

    // A material is parsed in two ways
    // - A material has standard textures assigned in the scene file, such as
    //   diffuse, normal, transparency maps. They map to predefined variable
    //   names.
    // - A material requires a specific texture, such as noise, or any other
    // texture of specific use for this material only.
    //   In that case,
    // diffuse texture
    if (geoMaterial.diffuseTexture != nullptr) {
      renderSystem.attachTexture(renderable, *geoMaterial.diffuseTexture, "diffuseTex");
    }
    if (geoMaterial.specularTexture != nullptr) {
      renderSystem.attachTexture(renderable, *geoMaterial.specularTexture, "specularTex");
    }
    if (geoMaterial.normalTexture != nullptr) {
      renderSystem.attachTexture(renderable, *geoMaterial.normalTexture, "normalTex");
    }
    if (geoMaterial.roughnessTexture != nullptr) {
      renderSystem.attachTexture(renderable, *geoMaterial.roughnessTexture, "normalTex");
    }
  }
}
