//
// Created by jlemein on 05-08-21.
//
#include <izzgl_materialsystem.h>

#include <wsp_workspace.h>
#include "izz_resourcemanager.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_rendersystem.h"
#include "izzgl_textureloader.h"
#include "izzgl_texturesystem.h"
#include <izzgl_shadersystem.h>
#include "uniform_constant.h"
#include "uniform_lambert.h"
#include "uniform_parallax.h"
#include "uniform_ubermaterial.h"

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include "uniform_blinnphong.h"
#include "uniform_blinnphongsimple.h"
#include "uniform_mvp.h"

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

MaterialSystem::MaterialSystem(entt::registry& registry, std::shared_ptr<lsw::ResourceManager> resourceManager)
  : m_registry{registry}
  , m_resourceManager{resourceManager} {
  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBlockManagers[izz::ufm::Lambert::PARAM_NAME] = std::make_unique<izz::ufm::LambertUniformManager>();
  m_uniformBlockManagers[izz::ufm::Parallax::PARAM_NAME] = std::make_unique<izz::ufm::ParallaxManager>();
  m_uniformBlockManagers[izz::ufm::Uber::PARAM_NAME] = std::make_unique<izz::ufm::UberUniformManager>();
  m_uniformBlockManagers[izz::ufm::ConstantLight::PARAM_NAME] = std::make_unique<izz::ufm::ConstantManager>();
  m_uniformBlockManagers[izz::ufm::BlinnPhong::PARAM_NAME] = std::make_unique<izz::ufm::BlinnPhongManager>();
  m_uniformBlockManagers[izz::ufm::BlinnPhongSimple::PARAM_NAME] = std::make_unique<izz::ufm::BlinnPhongSimpleManager>();
  m_uniformBlockManagers[izz::ufm::ModelViewProjection::PARAM_NAME] = std::make_unique<izz::ufm::MvpManager>();
}

UniformBuffer MaterialSystem::createUniformBuffer(const Material& m, const char* name, void* pData, std::size_t size) {
  spdlog::info("MaterialSystem: creating UBO '{}' for material {}", name, m.name);

  UniformBuffer ub {};
  ub.data = pData;
  ub.size = size;

  glGenBuffers(1, &ub.bufferId);
  glBindBuffer(GL_UNIFORM_BUFFER, ub.bufferId);

  glUseProgram(m.programId);
  ub.blockIndex = glGetUniformBlockIndex(m.programId, name);
  if (ub.blockIndex == GL_INVALID_INDEX) {
    throw std::runtime_error(fmt::format("Could not find UBO with name {}", name));
  }
  glGetActiveUniformBlockiv(m.programId, ub.blockIndex, GL_UNIFORM_BLOCK_BINDING, &ub.blockBind);

  glBindBufferBase(GL_UNIFORM_BUFFER, ub.blockBind, ub.bufferId);
  glBufferData(GL_UNIFORM_BUFFER, ub.size, NULL, GL_DYNAMIC_DRAW);

  return ub;

  //  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);
  //  glBufferData(GL_UNIFORM_BUFFER, renderable.pUboLightStructSize, nullptr, GL_DYNAMIC_DRAW);

      //      uniformBlocks[name].bufferId = TODO: fix this
      //      memcpy(properties[name].data, pData, size);
//    }
//    uniformBlocks.at(name).size = size;
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
            case PropertyType::TEXTURE2D: {
              auto texture = m_resourceManager->getTextureSystem()->loadTexture(value.get<std::string>());
              mat.setTexture(key, texture);
              break;
            }

            case PropertyType::FLOAT4:
              mat.userProperties.setFloatArray(key, value.get<std::vector<float>>());
              break;

            case PropertyType::FLOAT:
              mat.userProperties.setFloat(key, value.get<float>());
              break;

            case PropertyType::INT:
              mat.userProperties.setInt(key, value.get<int>());
              break;

            default:
              spdlog::warn("Material property {} is not part of the material definition. Ignored.", key);
              break;
          }
        } catch (std::out_of_range&) {
          throw std::runtime_error(
              fmt::format("Material instance '{}': property '{}' is not part of material definition '{}'.", instanceName, key, materialDefinition));
        }
      }
    }
  }
}

void addTextureToMaterial(const std::string& textureName, const std::string& path, izz::gl::Material& material) {
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

    if (material.contains("textures")) {
      for (const auto& [name, value] : material["textures"].items()) {
        if (!value.contains("type")) {
          spdlog::warn("Material '{}': texture misses required attributes 'type'. Property will be ignored.", m.name);
          continue;
        }

        std::string type = value["type"];

        if (type == "texture") {
          m.propertyTypes[name] = PropertyType::TEXTURE2D;
          std::string path = "";

          if (value.contains("default_value")) {
            path = value["default_value"].get<std::string>();
            m.setTexture(name, m_resourceManager->getTextureSystem()->loadTexture(path));
          } else {
            m.setTexture(name, nullptr);
          }
          spdlog::debug("\t{} texture: {}", name, path);
//          addTextureToMaterial(name, path, m);

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
              m.propertyTypes[name] = PropertyType::UNIFORM_BUFFER_OBJECT;

              std::size_t sizeOfStruct = 0;

              auto uniformData = m_uniformBlockManagers.at(name)->CreateUniformBlock(sizeOfStruct);
              m.setUniformBuffer(name.c_str(), UniformBuffer{.size = sizeOfStruct, .data = uniformData});
//              m.registerUniformBlock(name.c_str(), uniformData, sizeOfStruct);
              m.userProperties.ubo_name = name;

              for (const auto& [key, value] : value.items()) {
                if (value.is_array() && isFloatArray<>(value)) {
                  m.propertyTypes[key] = PropertyType::FLOAT4;
                  auto list = value.get<std::vector<float>>();
                  spdlog::debug(fmt::format("\t{}::{} = [{}]", name, key, fmt::join(list.begin(), list.end(), ", ")));
                  m.userProperties.setFloatArray(key, value.get<std::vector<float>>());
                } else if (value.is_number_float()) {
                  m.propertyTypes[key] = PropertyType::FLOAT;
                  spdlog::debug("\t{}::{} = {}", name, key, value.get<float>());
                  m.userProperties.setFloat(key, value.get<float>());
                } else if (value.is_number_integer()) {
                  m.propertyTypes[key] = PropertyType::INT;
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

  spdlog::debug("READING MATERIALS [DONE]");
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

namespace {
int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
  int location = glGetUniformLocation(program, name);
  if (location == -1) {
    throw std::runtime_error(fmt::format("unknown uniform parameter '{}' (material: '{}').", name, materialName));
  }
  return location;
}
}  // namespace

void MaterialSystem::allocateBuffers(Material& material) {
  // compile shader
  ShaderSystem shaderCompiler;
  if (material.isBinaryShader) {
    material.programId = shaderCompiler.compileSpirvShader(material.vertexShader, material.fragmentShader);
  } else {
    material.programId = shaderCompiler.compileShader(material.vertexShader, material.fragmentShader);
  }
  spdlog::debug("\tProgram id: {}\n\tShader program compiled successfully (vs: {} fs: {})", material.programId,
                material.vertexShader, material.fragmentShader);

  // load textures
  for (const auto& [texname, filepath] : material.texturePaths) {
    //    for (const auto& [texname, filepath] : material.texturePaths) {
    spdlog::debug("Loading Texture {}: {}", texname, filepath);

    Texture* pTexture {nullptr};
    if (!filepath.empty()) {
      pTexture = m_resourceManager->getTextureSystem()->loadTexture(filepath);
    }

    TextureBuffer tb;
    tb.textureId = pTexture != nullptr ? pTexture->bufferId : -1;
    tb.location = glGetUniformLocation(material.programId, texname.c_str());

    if (tb.location != GL_INVALID_INDEX) {
      spdlog::debug("Material {}: added texture {}: '{}'", material.name, texname, filepath);
      material.textures[texname] = tb;
    } else {
      spdlog::warn("Material {} defines texture \"{}\", but it is not found in the shader.", material.name, texname);
    }
  }

  // load already loaded textures
  for (auto& [name, textureBuffer] : material.textures) {
    textureBuffer.location = glGetUniformLocation(material.programId, name.c_str());
  }

  for (auto& [name, ubo] : material.uniformBuffers) {
    spdlog::debug("MaterialSystem: allocating uniform buffer on GPU");
    ubo = createUniformBuffer(material, name.c_str(), ubo.data, ubo.size);
  }

  // INIT UNSCOPED UNIFORMS
  //  first memory allocation. For this we need to know the number of properties and length of data properties.
  int numProperties = material.unscopedUniforms.booleanValues.size() + material.unscopedUniforms.intValues.size() +
                      material.unscopedUniforms.floatValues.size() + material.unscopedUniforms.floatArrayValues.size();
  uint64_t sizeBytes = 0U;
  sizeBytes += material.unscopedUniforms.booleanValues.size() * sizeof(GLint);
  sizeBytes += material.unscopedUniforms.intValues.size() * sizeof(GLint);
  sizeBytes += material.unscopedUniforms.floatValues.size() * sizeof(GLfloat);
  for (const auto& array : material.unscopedUniforms.floatArrayValues) {
    sizeBytes += array.second.size() * sizeof(GLfloat);
  }

  material.unscopedUniformBuffer = UnscopedUniforms::Allocate(numProperties, sizeBytes);

  auto pData = material.unscopedUniformBuffer.pData;
  auto pUniform = material.unscopedUniformBuffer.pProperties;
  unsigned int offset = 0U;

  for (auto [name, value] : material.unscopedUniforms.booleanValues) {
    *reinterpret_cast<int*>(pData) = value;
    pUniform->location = getUniformLocation(material.programId, name.c_str(), material.name);
    pUniform->type = UType::BOOL;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1i(pUniform->location, *reinterpret_cast<int*>(material.unscopedUniformBuffer.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(GLint);
    pData += sizeof(GLint);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.intValues) {
    *reinterpret_cast<int*>(pData) = value;
    pUniform->location = getUniformLocation(material.programId, name.c_str(), material.name);
    pUniform->type = UType::INT;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1i(pUniform->location, *reinterpret_cast<int*>(material.unscopedUniformBuffer.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(GLint);
    pData += sizeof(GLint);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.floatValues) {
    *reinterpret_cast<float*>(pData) = value;
    pUniform->location = getUniformLocation(material.programId, name.c_str(), material.name);
    pUniform->type = UType::FLOAT;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1f(pUniform->location, *reinterpret_cast<float*>(material.unscopedUniformBuffer.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(float);
    pData += sizeof(float);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
    memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
    pUniform->location = getUniformLocation(material.programId, name.c_str(), material.name);
    pUniform->type = UType::FLOAT_ARRAY;
    pUniform->offset = offset;
    pUniform->length = value.size();

    glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(material.unscopedUniformBuffer.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));

    offset += sizeof(float) * value.size();
    pData += sizeof(float) * value.size();
    pUniform++;
  }
}

Material& MaterialSystem::createMaterial(const std::string& name) {
  Material material;

  if (m_materialInstances.count(name) > 0) {
    material = m_materialInstances.at(name);
    allocateBuffers(material);
    material.id = static_cast<MaterialId>(m_createdMaterials.size() + 1);
    m_createdMaterials[material.id] = material;
    return m_createdMaterials.at(material.id);
  } else {
    // 1. First process material mapping to material name
    auto materialName = m_materialMappings.count(name) > 0 ? m_materialMappings.at(name) : name;

    // 2. Match the specified name with a known material
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
  }

  allocateBuffers(material);

  material.id = static_cast<MaterialId>(m_createdMaterials.size() + 1);
  m_createdMaterials[material.id] = material;
  return m_createdMaterials.at(material.id);
}

izz::gl::Material& MaterialSystem::makeDefaultMaterial() {
  return createMaterial(m_defaultMaterial);
}

izz::gl::Material& MaterialSystem::getMaterialById(int id) {
  try {
    return m_createdMaterials.at(id);
  } catch (std::out_of_range&) {
    throw std::runtime_error(fmt::format("Could not find material with id {}", id));
  }
}

void MaterialSystem::update(float time, float dt) {
//  auto view = m_registry.view<izz::gl::Material, Renderable>();

  for (auto& m : m_createdMaterials) {
    for (const auto& [name, uniformBlock] : m.second.uniformBuffers) {
#ifndef NDEBUG
      if (m_uniformBlockManagers.count(name) <= 0) {
        throw std::runtime_error(fmt::format("Material system cannot find manager for ubo block '{}'", name));
      }
#endif  // NDEBUG

      m_uniformBlockManagers.at(name)->UpdateUniform(uniformBlock.data, m.second);
    }
  }
}

void MaterialSystem::synchronizeTextures(RenderSystem& renderSystem) {
//  auto view = m_registry.view<izz::gl::Material>();  // why are we not requesting
//                                                      // <Material, Renderable>?
//
//  // put the code in bottom part of readMaterialDefinitions here
//  // ...
//
//  for (auto entity : view) {
//    auto& renderable = m_registry.get_or_emplace<Renderable>(entity);
//    auto& geoMaterial = view.get<izz::gl::Material>(entity);
//    spdlog::debug("Attach textures for material '{}' to render system", geoMaterial.name);
//
//    for (auto& [name, path] : geoMaterial.texturePaths) {
//      // TODO: check if name is used in shader object
//      try {
//        auto t = m_resourceManager->getTextureSystem()->loadTexture(path);
//        if (t != nullptr) {
//          geoMaterial.textures[name] = t->id;
//          renderSystem.attachTexture(renderable, *t, name);
//        }
//      } catch (std::out_of_range e) {
//        throw std::runtime_error(fmt::format("Cannot find texture with name \"{}\" in material \"{}\"", name, geoMaterial.name));
//      }
//    }
//
//    // A material is parsed in two ways
//    // - A material has standard textures assigned in the scene file, such as
//    //   diffuse, normal, transparency maps. They map to predefined variable
//    //   names.
//    // - A material requires a specific texture, such as noise, or any other
//    // texture of specific use for this material only.
//    //   In that case,
//    // diffuse texture
//    if (geoMaterial.diffuseTexture != nullptr) {
//      renderSystem.attachTexture(renderable, *geoMaterial.diffuseTexture, "diffuseTex");
//    }
//    if (geoMaterial.specularTexture != nullptr) {
//      renderSystem.attachTexture(renderable, *geoMaterial.specularTexture, "specularTex");
//    }
//    if (geoMaterial.normalTexture != nullptr) {
//      renderSystem.attachTexture(renderable, *geoMaterial.normalTexture, "normalTex");
//    }
//    if (geoMaterial.roughnessTexture != nullptr) {
//      renderSystem.attachTexture(renderable, *geoMaterial.roughnessTexture, "normalTex");
//    }
//  }
}
