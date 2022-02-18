//
// Created by jlemein on 05-08-21.
//
#include <izzgl_materialsystem.h>

#include "izz_scenegraphhelper.h"
#include "geo_textureloader.h"
#include "gl_rendersystem.h"
#include "izz_resourcemanager.h"
#include "izz_texturesystem.h"
#include "uniform_constant.h"
#include "uniform_lambert.h"
#include "uniform_parallax.h"
#include "uniform_ubermaterial.h"
#include <wsp_workspace.h>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include "uniform_blinnphong.h"
#include "uniform_blinnphongsimple.h"

using json = nlohmann::json;

using namespace izz;
using namespace izz::gl;

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

MaterialSystem::MaterialSystem(std::shared_ptr<izz::SceneGraphHelper> sceneGraph, std::shared_ptr<lsw::ResourceManager> resourceManager)
  : m_sceneGraph{sceneGraph}
  , m_resourceManager{resourceManager} {
  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBlockManagers[lsw::ufm::Lambert::PARAM_NAME] = std::make_unique<lsw::ufm::LambertUniformManager>();
  m_uniformBlockManagers[lsw::ufm::Parallax::PARAM_NAME] = std::make_unique<lsw::ufm::ParallaxManager>();
  m_uniformBlockManagers[lsw::ufm::Uber::PARAM_NAME] = std::make_unique<lsw::ufm::UberUniformManager>();
  m_uniformBlockManagers[lsw::ufm::ConstantLight::PARAM_NAME] = std::make_unique<lsw::ufm::ConstantManager>();
  m_uniformBlockManagers[lsw::ufm::BlinnPhong::PARAM_NAME] = std::make_unique<lsw::ufm::BlinnPhongManager>();
  m_uniformBlockManagers[lsw::ufm::BlinnPhongSimple::PARAM_NAME] = std::make_unique<lsw::ufm::BlinnPhongSimpleManager>();
}

void MaterialSystem::readMaterialInstances(json& j) {
  for (auto md : j["material_instances"]) {
    auto instanceName = md["name"].get<std::string>();
    auto materialDefinition = md["material_definition"].get<std::string>();

    if (instanceName.empty() || materialDefinition.empty()) {
      throw std::runtime_error("Failed to read material instance");
    }
    if (m_materials.count(materialDefinition) <= 0) {
      throw std::runtime_error(
          fmt::format("Material instance '{}', instanced from '{}' does not occur in the list of material definitions.", instanceName, materialDefinition));
    }

    m_materialInstances[instanceName] = m_materials[materialDefinition];
    auto& mat = m_materialInstances[instanceName];

    if (md.contains("properties")) {
      auto properties = md["properties"];
      for (const auto& [key, value] : properties.items()) {
        try {
          auto type = mat.propertyTypes.at(key);
          switch (type) {
            case lsw::geo::Material::PropertyType::TEXTURE2D:
              mat.setTexture(key, value.get<std::string>());
              break;

            case lsw::geo::Material::PropertyType::FLOAT4:
              mat.userProperties.setFloatArray(key, value.get<std::vector<float>>());
              break;

            case lsw::geo::Material::PropertyType::FLOAT:
              mat.userProperties.setFloat(key, value.get<float>());
              break;

            case lsw::geo::Material::PropertyType::INT:
              mat.userProperties.setInt(key, value.get<int>());
              break;

            default:
              spdlog::warn("Material property {} is not part of the material definition. Ignored.", key);
              break;
          }
        } catch (std::out_of_range&) {
          throw std::runtime_error(
              fmt::format("Property '{}' of instance '{}' is not part of material definition '{}'.", key, instanceName, materialDefinition));
        }
      }
    }
  }
}

void addTextureToMaterial(const std::string& textureName, const std::string& path, lsw::geo::Material& material) {
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
    lsw::geo::Material m;
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

    if (material.contains("textures")) {
      for (const auto& [name, value] : material["textures"].items()) {
        if (!value.contains("type")) {
          spdlog::warn("Material '{}': texture misses required attributes 'type'. Property will be ignored.", m.name);
          continue;
        }

        std::string type = value["type"];

        if (type == "texture") {
          m.propertyTypes[name] = lsw::geo::Material::TEXTURE2D;
          auto path = value["default_value"].get<std::string>();
          spdlog::debug("\t{} texture: {}", name, path);
          addTextureToMaterial(name, path, m);
        } else {
          spdlog::warn("Material '{}' defines property {} of type {} that is not supported. Property will be ignored.", m.name, name, type);
        }
      }
    }

    if (material.contains("properties")) {
      // dealing with uniform props not in an interface block
      for (const auto& [key, value] : material["properties"].items()) {
        if (value.is_boolean()) {
          m.unscopedUniforms.setBoolean(key, value.get<bool>());
        } else if (value.is_number_float()) {
          m.unscopedUniforms.setFloat(key, value.get<float>());
        } else if (value.is_number_integer()) {
          m.unscopedUniforms.setInt(key, value.get<int>());
        } else if (value.is_array() && isFloatArray<>(value)) {
          m.unscopedUniforms.setFloatArray(key, value.get<std::vector<float>>());
        } else if (value.is_object()) {
          // scoped uniform (i.e. using interface block)
          auto name = key;

          try {
            if (m_uniformBlockManagers.count(name) <= 0) {
              throw std::runtime_error(fmt::format(
                  "{} with uniform block '{}': material system does not contain an UBO handler for this UBO name. Shader will likely misbehave.", m.name,
                  name));
            } else {
              m.propertyTypes[name] = lsw::geo::Material::PropertyType::UNIFORM_BUFFER_OBJECT;

              std::size_t sizeOfStruct = 0;
              auto uniformData = m_uniformBlockManagers.at(name)->CreateUniformBlock(sizeOfStruct);
              m.registerUniformBlock(name.c_str(), uniformData, sizeOfStruct);
              m.userProperties.ubo_name = name;

              for (const auto& [key, value] : value.items()) {
                if (value.is_array() && isFloatArray<>(value)) {
                  m.propertyTypes[key] = lsw::geo::Material::PropertyType::FLOAT4;
                  auto list = value.get<std::vector<float>>();
                  spdlog::debug(fmt::format("\t{}::{} = [{}]", name, key, fmt::join(list.begin(), list.end(), ", ")));
                  m.userProperties.setFloatArray(key, value.get<std::vector<float>>());
                } else if (value.is_number_float()) {
                  m.propertyTypes[key] = lsw::geo::Material::PropertyType::FLOAT;
                  spdlog::debug("\t{}::{} = {}", name, key, value.get<float>());
                  m.userProperties.setFloat(key, value.get<float>());
                } else if (value.is_number_integer()) {
                  m.propertyTypes[key] = lsw::geo::Material::PropertyType::INT;
                  spdlog::debug("\t{}::{} = {}", name, key, value.get<int>());
                  m.userProperties.setInt(key, value.get<int>());
                } else {
                  spdlog::error("Material {} with parameter '{}' is ignored. Data type is not supported.", m.name, key);
                }
              }
            }

          } catch (std::runtime_error& e) {
            spdlog::error("Failed to parse property {} for material {}, details: {}", name, m.name, e.what());
            throw e;
          }
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
  readMaterialInstances(j);
//  readEffects(j);

  if (j.contains("default_material")) {
    auto defaultMaterial = j["default_material"];
    if (m_materials.count(defaultMaterial) > 0) {
      m_defaultMaterial = defaultMaterial;
    } else {
      spdlog::warn("{} specifies a default material '{}' that is not defined in the list of materials", path, defaultMaterial);
    }
  }

  input.close();
}

bool MaterialSystem::isMaterialDefined(const std::string& materialName) {
  return m_materialMappings.count(materialName) > 0 || m_materials.count(materialName) > 0;
}

std::shared_ptr<lsw::geo::Material> MaterialSystem::createMaterial(const std::string& name) {
  if (m_materialInstances.count(name) > 0) {
    return std::make_shared<lsw::geo::Material>(m_materialInstances.at(name));
  }

  // 1. First process material mapping to material name
  auto materialName = m_materialMappings.count(name) > 0 ? m_materialMappings.at(name) : name;

  // 2. Match the specified name with a known material
  lsw::geo::Material material;
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

  return std::make_shared<lsw::geo::Material>(material);
}

std::shared_ptr<lsw::geo::Material> MaterialSystem::makeDefaultMaterial() {
  return createMaterial(m_defaultMaterial);
}

void MaterialSystem::update(float time, float dt) {
  auto view = m_sceneGraph->getRegistry().view<lsw::geo::Material, Renderable>();
  for (auto e : view) {
    const auto& r = view.get<Renderable>(e);
    const auto& m = view.get<lsw::geo::Material>(e);

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

void MaterialSystem::synchronizeTextures(RenderSystem& renderSystem) {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<lsw::geo::Material>();  // why are we not requesting
                                               // <Material, Renderable>?

  // put the code in bottom part of readMaterialDefinitions here
  // ...

  for (auto entity : view) {
    auto& renderable = registry.get_or_emplace<Renderable>(entity);
    auto& geoMaterial = view.get<lsw::geo::Material>(entity);
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
