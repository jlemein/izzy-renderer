//
// Created by jlemein on 05-08-21.
//
#include <izzgl_materialsystem.h>

#include <izzgl_shadersystem.h>
#include <wsp_workspace.h>
#include "izz_resourcemanager.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_rendersystem.h"
#include "izzgl_textureloader.h"
#include "izzgl_texturesystem.h"
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

int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
  int location = glGetUniformLocation(program, name);
  if (location == -1) {
    spdlog::warn(
        fmt::format("{}: unknown uniform parameter '{}' (material: '{}'). Is parameter present and used in shader?", Material::ID, name, materialName));
  }
  return location;
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

void MaterialSystem::addMaterialDescription(std::string name, izz::geo::MaterialDescription materialDescription) {
  m_materialDescriptions[name] = materialDescription;
}

UniformBuffer MaterialSystem::createUniformBuffer(const izz::geo::UniformBufferDescription& bufferDescription, const Material& m) {
  spdlog::info("MaterialSystem: creating UBO '{}' for material {}", bufferDescription.name, m.name);

  UniformBuffer ub{};
  ub.data = m_uniformBlockManagers.at(bufferDescription.name)->CreateUniformBlock(ub.size);

  glGenBuffers(1, &ub.bufferId);
  glBindBuffer(GL_UNIFORM_BUFFER, ub.bufferId);

  glUseProgram(m.programId);  // might not be needed
  ub.blockIndex = glGetUniformBlockIndex(m.programId, bufferDescription.name.c_str());
  if (ub.blockIndex == GL_INVALID_INDEX) {
    throw std::runtime_error(fmt::format("Could not find UBO with name {}", bufferDescription.name));
  }
  glGetActiveUniformBlockiv(m.programId, ub.blockIndex, GL_UNIFORM_BLOCK_BINDING, &ub.blockBind);

  glBindBufferBase(GL_UNIFORM_BUFFER, ub.blockBind, ub.bufferId);
  glBufferData(GL_UNIFORM_BUFFER, ub.size, NULL, GL_DYNAMIC_DRAW);

  return ub;
}

izz::geo::MaterialDescription& MaterialSystem::getMaterialDescription(const std::string& name) {
  return m_materialDescriptions.at(name);
}

bool MaterialSystem::hasMaterialDescription(const std::string& materialName) {
  return m_materialMappings.count(materialName) > 0 || m_materialDescriptions.count(materialName) > 0;
}

void MaterialSystem::setDefaultMaterial(const std::string& name) {
  if (m_materialDescriptions.count(name) > 0) {
    m_defaultMaterial = name;
  } else {
    spdlog::warn("Setting default material '{}' failed, because material system failed to find a material description with that name.");
  }
}

void MaterialSystem::compileShader(Material& material, const izz::geo::MaterialDescription& materialDescription) {
  // compile shader
  ShaderSystem shaderCompiler;
  if (materialDescription.isBinaryShader) {
    material.programId = shaderCompiler.compileSpirvShader(materialDescription.vertexShader, materialDescription.fragmentShader);
  } else {
    material.programId = shaderCompiler.compileShader(materialDescription.vertexShader, materialDescription.fragmentShader);
  }
  spdlog::debug("\tProgram id: {}\n\tShader program compiled successfully (vs: {} fs: {})", material.programId, materialDescription.vertexShader,
                materialDescription.fragmentShader);
}

void MaterialSystem::allocateTextureBuffers(Material& material, const std::unordered_map<std::string, izz::geo::TextureDescription>& textureDescriptions) {
  for (const auto& [textureName, texture] : textureDescriptions) {
    spdlog::debug("Loading Texture {}: {}", textureName, texture.path.string());

    Texture* pTexture{nullptr};
    if (!texture.path.empty()) {
      pTexture = m_resourceManager->getTextureSystem()->loadTexture(texture.path);
    }

    TextureBuffer tb;
    tb.textureId = pTexture != nullptr ? pTexture->bufferId : -1;
    tb.location = glGetUniformLocation(material.programId, textureName.c_str());

    if (tb.location != GL_INVALID_INDEX) {
      spdlog::debug("Material {}: added texture {}: '{}'", material.name, textureName, texture.path);
      material.textures[textureName] = tb;
    } else {
      spdlog::warn("Material {} defines texture \"{}\", but it is not found in the shader.", material.name, textureName);
    }
  }
}

void MaterialSystem::allocateBuffers(Material& material, const izz::geo::MaterialDescription& materialDescription) {
  // allocate texture buffers based on texture descriptions.
  allocateTextureBuffers(material, materialDescription.textures);

  for (auto& [name, uboDescription] : materialDescription.uniformBuffers) {
    spdlog::debug("MaterialSystem: allocating uniform buffer on GPU");
    material.uniformBuffers[name] = createUniformBuffer(uboDescription, material);
  }

  // INIT UNSCOPED UNIFORMS
  //  first memory allocation. For this we need to know the number of properties and length of data properties.
  uint64_t sizeBytes = 0U;
  int numProperties = 0;
  for (const auto& [name, uniform] : materialDescription.uniforms) {
    switch (uniform.type) {
      case izz::geo::PropertyType::FLOAT:
      case izz::geo::PropertyType::INT:
      case izz::geo::PropertyType::BOOL:
        ++numProperties;
        sizeBytes += sizeof(GLint);
        break;

      case izz::geo::PropertyType::FLOAT3:
        ++numProperties;
        sizeBytes += 3 * sizeof(GLfloat);
        break;

      case izz::geo::PropertyType::FLOAT4:
        ++numProperties;
        sizeBytes += 4 * sizeof(GLfloat);
        break;

      case izz::geo::PropertyType::FLOAT_ARRAY:
        ++numProperties;
        sizeBytes += uniform.length * sizeof(GLfloat);
        break;

      case izz::geo::PropertyType::UNIFORM_BUFFER_OBJECT:
      case izz::geo::PropertyType::TEXTURE2D:
        break;

      default:
        spdlog::warn("Uniform property {} is ignored", name);
    }
  }
  //  int numProperties = material.unscopedUniforms.booleanValues.size() + material.unscopedUniforms.intValues.size() +
  //                      material.unscopedUniforms.floatValues.size() + material.unscopedUniforms.floatArrayValues.size();
  //  uint64_t sizeBytes = 0U;
  //  sizeBytes += material.unscopedUniforms.booleanValues.size() * sizeof(GLint);
  //  sizeBytes += material.unscopedUniforms.intValues.size() * sizeof(GLint);
  //  sizeBytes += material.unscopedUniforms.floatValues.size() * sizeof(GLfloat);
  //  for (const auto& array : material.unscopedUniforms.floatArrayValues) {
  //    sizeBytes += array.second.size() * sizeof(GLfloat);
  //  }

  // allocate enough storage room to store uniform properties.
  material.globalUniforms = std::make_shared<UniformProperties>(sizeBytes, numProperties);

  // TODO: useProgram

  for (const auto& [name, p] : materialDescription.uniforms) {
    bool isGlobalUniform = (name.find(".") == std::string::npos);
    auto& uniforms = isGlobalUniform ? material.globalUniforms : material.scopedUniforms;
    auto location = getUniformLocation(material.programId, name.c_str(), material.name);
    UniformProperty* prop;

    switch (p.type) {
      case geo::PropertyType::BOOL:
        prop = uniforms->addBoolean(name, std::get<bool>(p.value), location);
        material.m_allUniforms[name] = prop;

//        material.addUniformBool(name, std::get<bool>(p.value));
        spdlog::info("Initialized: {}.{} = {}", material.name, name, std::get<int>(p.value));
        break;

      case geo::PropertyType::INT:
        prop = uniforms->addInt(name, std::get<int>(p.value), location);
        material.m_allUniforms[name] = prop;

//        material.addUniformInt(name, std::get<int>(p.value));
        spdlog::info("Initialized: {}.{} = {}", material.name, name, std::get<int>(p.value));
        break;

      case geo::PropertyType::FLOAT:
        prop = uniforms->addFloat(name, std::get<float>(p.value), location);
        material.m_allUniforms[name] = prop;

//        material.addUniformFloat(name, std::get<float>(p.value));
        spdlog::info("Initialized: {}.{} = {}", material.name, name, std::get<float>(p.value));
        break;

      case geo::PropertyType::FLOAT_ARRAY: {
        prop = uniforms->addFloatArray(name, std::get<std::vector<float>>(p.value), location);
        material.m_allUniforms[name] = prop;

//        material.addUniformFloatArray(name, std::get<std::vector<float>>(p.value));
        break;
      }

      default:
        spdlog::warn("{}: could not set uniform property '{}'. Data type not supported.", ID, p.name);
        break;
    }
  }
}

izz::geo::MaterialDescription& MaterialSystem::resolveMaterialDescription(const std::string name) {
  // 1. Map the material name to a different material name
  auto materialName = m_materialMappings.count(name) > 0 ? m_materialMappings.at(name) : name;

  if (m_materialDescriptions.count(materialName) == 0) {
    spdlog::warn("Material with name '{}' not found, using default material {}.", materialName, m_defaultMaterial);

    // use a default material, if the "mapped-to-material" is not defined.
    materialName = m_defaultMaterial;

    // if also the default material is not defined
    if (m_materialDescriptions.count(m_defaultMaterial) == 0 && m_materialDescriptions.empty()) {
      throw std::runtime_error(fmt::format("Failed to create material '{}': no such material is defined.", materialName));
    } else {
      materialName = m_materialDescriptions.begin()->first;
    }
  }

  return m_materialDescriptions.at(materialName);
}

Material& MaterialSystem::createMaterial(const std::string& name) {
  auto& material = createMaterial(resolveMaterialDescription(name));
  material.name = name;
  return material;
}

Material& MaterialSystem::createMaterial(const izz::geo::MaterialDescription& materialDescription) {
  Material material;
  material.name = materialDescription.vertexShader;
  compileShader(material, materialDescription);

  allocateBuffers(material, materialDescription);

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
