//
// Created by jlemein on 05-08-21.
//
#include <georm_materialsystem.h>

#include <ecsg_scenegraph.h>
#include <georm_resourcemanager.h>
#include <nlohmann/json.hpp>

#include <ecs_rendersystem.h>
#include <fstream>
#include <geo_textureloader.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <uniform_ubermaterial.h>
#include <uniform_lambert.h>
#include <uniform_parallax.h>

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

}  // namespace

MaterialSystem::MaterialSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph,
                               std::shared_ptr<georm::ResourceManager> resourceManager)
  : m_sceneGraph{sceneGraph}
  , m_resourceManager{resourceManager} {

  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBlockManagers[ufm::Lambert::PARAM_NAME] = std::make_unique<ufm::LambertUniformManager>();
  m_uniformBlockManagers[ufm::Parallax::PARAM_NAME] = std::make_unique<ufm::ParallaxManager>();
  m_uniformBlockManagers[ufm::Uber::PARAM_NAME] = std::make_unique<ufm::UberUniformManager>();
}

void MaterialSystem::readMaterialMappings(json& j) {
  for (auto md : j["material_mapping"]) {
    auto fromMaterial = md["from"].get<std::string>();
    auto toMaterial = md["to"].get<std::string>();

    if (fromMaterial.empty() || toMaterial.empty()) {
      throw std::runtime_error("Failed to read material mappings");
    }
    if (m_materials.count(toMaterial) <= 0) {
      throw std::runtime_error(
          fmt::format("Material mapping '{}' -> '{}' does not occur in the list of materials", fromMaterial, toMaterial));
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

void MaterialSystem::readMaterialDefinitions(nlohmann::json& j) {
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
      m.vertexShader = material["vertex_shader"].get<std::string>();
      m.geometryShader = material["geometry_shader"].get<std::string>();
      m.fragmentShader = material["fragment_shader"].get<std::string>();

      spdlog::debug("\tvertex shader: {}", m.vertexShader);
      spdlog::debug("\tgeometry shader: {}", m.geometryShader);
      spdlog::debug("\tfragment shader: {}", m.fragmentShader);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(fmt::format("Failed parsing shader names: {}", e.what()));
    }

    // load generic attributes
    auto& properties = material["properties"];
    for (const auto& prop : properties) {
      // every property always has a name and type
      std::string name = prop["name"];
      std::string type = prop["type"];

      try {
        if (type == "uniform") {
          auto value = prop["value"];
          for (const auto& [key, value] : value.items()) {
            spdlog::error("Uniform block {} --> {}: {} -- IGNORED FOR NOW", name, key, value);
//            m.setProperty(name.c_str(), )
            // TODO: register the name of the uniform block. Create a UniformPropertiesBlock that provides the material
            //  interface to set values. Or consider that a material can have only one uniform block for material properties.
            //  That means the shader has a Lighting, Matrices and Material uniform block of which the latter one is the one that
            //  is the target object for setProperty calls.
//            m.uniformBlocks.at(name).setFloatArray(key, value);
          }
        } else if (type == "texture") {
          auto path = prop["value"].get<std::string>();
          spdlog::debug("\t{} texture: {}", name, path);
          addTextureToMaterial(name, path, m);
        } else {
          spdlog::warn("Material '{}' defines property {} of type {} that is not supported. Property will be ignored.", m.name,
                       name, type);
        }

      } catch (std::runtime_error& e) {
        spdlog::error("Failed to parse property {} for material {}, details: {}", name, m.name, e.what());
        throw e;
      }
    }

    //        else {
    //        if (prop["value"].is_array()) {
    //            auto floatArray = prop["value"].get<std::vector<float>>();
    //            m.setFloatArray(name, floatArray);
    //            spdlog::debug("\t{}: [{}, {}, {}]", name, floatArray[0], floatArray[1], floatArray[2]);
    //          } else {
    //            auto value = prop["value"].get<float>();
    //            m.setFloat(name, value);
    //            m.setProperty(name.c_str(), nullptr);
    //            spdlog::debug("\t{}: {}", name, value);
    //          }
    //        }

    // TODO: for now we parse properties here
    // Every material property needs to be converted to a representation in
    // the shader unit. This is done using Uniform buffer objects. Some
    // materials might be able to reuse the data block. The data block is
    // specified with a layout attribute in the json file.
//    if (material.count("layout") > 0) {
//      auto layout = material["layout"].get<std::string>();
//
//      if (layout == "_Lambert") {
//        m.setProperty(ufm::LambertData::FromMaterial(m));
//      } else if (layout == "_UberMaterial") {
//        m.setProperty(UberMaterialData::FromMaterial(m));
//      } else {
//        spdlog::error(
//            "{} layout structure is unknown. Continue without "
//            "which may lead to shading anomalies.",
//            layout);
//      }
//    }

    m_materials[m.name] = m;  // std::move(m);
  }
}

void MaterialSystem::loadMaterialsFromFile(const std::string& path) {
  // create an empty structure (null)
  std::ifstream input(path);
  if (input.fail()) {
    auto msg = fmt::format("MaterialSystem fails to initialize: cannot read from '{}'", path);
    spdlog::error(msg);
    throw std::runtime_error(msg);
  }

  json j;
  input >> j;

  // initialization of material system needs to be done in order.
  // first material definitions are read.
  readMaterialDefinitions(j);
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

std::unique_ptr<res::IResource> MaterialSystem::createResource(const std::string& name) {
  auto materialName = m_materialMappings.count(name) > 0 ? m_materialMappings.at(name) : name;

  Material material;
  if (m_materials.count(materialName) > 0) {
    material = m_materials.at(materialName);
  } else {
    if (m_materials.count(m_defaultMaterial) > 0) {
      spdlog::warn("Material with name '{}' not found, default material used.", materialName);
      material = m_materials.at(m_defaultMaterial);
    } else {
      throw std::runtime_error(fmt::format("Failed to create material '{}': no such material is defined.", materialName));
    }
  }

  return std::make_unique<lsw::res::Resource<geo::Material>>(material);
}

MaterialPtr MaterialSystem::makeDefaultMaterial() {
  return m_resourceManager->createSharedMaterial(m_defaultMaterial);  // createResource(m_defaultMaterial);
}

void MaterialSystem::synchronizeTextures(ecs::RenderSystem& renderSystem) {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<geo::Material>();  // why are we not requesting
                                               // <Material, Renderable>?

  // put the code in bottom part of readMaterialDefinitions here
  // ...

  for (auto entity : view) {
    auto& renderable = registry.get_or_emplace<ecs::Renderable>(entity);
    auto& geoMaterial = view.get<geo::Material>(entity);

    for (auto& [name, path] : geoMaterial.texturePaths) {
      // TODO: check if name is used in shader object
      try {
        auto t = m_resourceManager->loadTexture(path);
        geoMaterial.textures[name] = t;
        renderSystem.attachTexture(renderable, **t, name);
      } catch (std::out_of_range e) {
        throw std::runtime_error(
            fmt::format("Cannot find texture with name \"{}\" in material \"{}\"", name, geoMaterial.name));
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
      renderSystem.attachTexture(renderable, **geoMaterial.diffuseTexture, "diffuseTex");
    }
    if (geoMaterial.specularTexture != nullptr) {
      renderSystem.attachTexture(renderable, **geoMaterial.specularTexture, "specularTex");
    }
    if (geoMaterial.normalTexture != nullptr) {
      renderSystem.attachTexture(renderable, **geoMaterial.normalTexture, "normalTex");
    }
    if (geoMaterial.roughnessTexture != nullptr) {
      renderSystem.attachTexture(renderable, **geoMaterial.roughnessTexture, "normalTex");
    }
  }
}
