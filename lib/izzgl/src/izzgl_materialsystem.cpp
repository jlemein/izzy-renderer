//
// Created by jlemein on 05-08-21.
//
#include <izzgl_materialsystem.h>

#include <izzgl_shadersystem.h>
#include "izz_resourcemanager.h"
#include "izz_scenegraphhelper.h"
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
#include "uniform_albedospecularity.h"
#include "uniform_blinnphong.h"
#include "uniform_blinnphongsimple.h"
#include "uniform_deferredlighting.h"
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

MaterialSystem::MaterialSystem(entt::registry& registry, std::shared_ptr<izz::ResourceManager> resourceManager)
  : m_registry{registry}
  , m_resourceManager{resourceManager} {
  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBlockManagers[izz::ufm::Lambert::PARAM_NAME] = std::make_unique<izz::ufm::LambertUniformManager>();
  m_uniformBlockManagers[izz::ufm::Parallax::PARAM_NAME] = std::make_unique<izz::ufm::ParallaxManager>();
  m_uniformBlockManagers[izz::ufm::Uber::PARAM_NAME] = std::make_unique<izz::ufm::UberUniformManager>();
  m_uniformBlockManagers[izz::ufm::ConstantLight::PARAM_NAME] = std::make_unique<izz::ufm::ConstantManager>();
  m_uniformBlockManagers[izz::ufm::AlbedoSpecularity::PARAM_NAME] = std::make_unique<izz::ufm::AlbedoSpecularityManager>();
  m_uniformBlockManagers[izz::ufm::BlinnPhong::PARAM_NAME] = std::make_unique<izz::ufm::BlinnPhongManager>();
  m_uniformBlockManagers[izz::ufm::BlinnPhongSimple::PARAM_NAME] = std::make_unique<izz::ufm::BlinnPhongSimpleManager>();
  m_uniformBlockManagers[izz::ufm::ModelViewProjection::PARAM_NAME] = std::make_unique<izz::ufm::MvpManager>();
  m_uniformBlockManagers[izz::ufm::DeferredLighting::PARAM_NAME] = std::make_unique<izz::ufm::DeferredLightingManager>();
}

void MaterialSystem::addMaterialTemplate(izz::geo::MaterialTemplate materialTemplate) {
  m_materialTemplates[materialTemplate.name] = materialTemplate;
}

UniformBuffer MaterialSystem::createUniformBuffer(const izz::geo::UniformBufferDescription& bufferDescription, const Material& m) {
  spdlog::info("MaterialSystem: creating UBO '{}' for material {}", bufferDescription.name, m.getName());

  if (m_uniformBlockManagers.count(bufferDescription.name) == 0) {
    throw std::runtime_error(fmt::format("No uniform buffer manager defined for uniform buffer '{}'", bufferDescription.name));
  }

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

bool MaterialSystem::hasMaterialTemplate(const std::string& materialTemplateName) {
  return m_materialMappings.count(materialTemplateName) > 0 || m_materialTemplates.count(materialTemplateName) > 0;
}

void MaterialSystem::setDefaultMaterial(const std::string& name) {
  if (m_materialTemplates.count(name) > 0) {
    m_defaultMaterialTemplateName = name;
  } else {
    spdlog::warn("Setting default material '{}' failed, because material system failed to find a material description with that name.");
  }
}

ShaderVariantInfo MaterialSystem::compileShader(const izz::geo::MaterialTemplate& materialTemplate) {
  ShaderVariantInfo info;
  ShaderCompiler shaderCompiler;

  if (materialTemplate.isBinaryShader) {
    info.programId = shaderCompiler.compileSpirvShader(materialTemplate.vertexShader, materialTemplate.fragmentShader);
  } else {
    info.programId = shaderCompiler.compileShader(materialTemplate.vertexShader, materialTemplate.fragmentShader);
  }
  spdlog::debug("\tProgram id: {}\n\tShader program compiled successfully (vs: {} fs: {})", info.programId, materialTemplate.vertexShader,
                materialTemplate.fragmentShader);

  info.materialTemplateName = materialTemplate.name;
  return info;
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
      spdlog::debug("Material {}: added texture {}: '{}'", material.getName(), textureName, texture.path);
      material.textures[textureName] = tb;
    } else {
      spdlog::warn("Material {} defines texture \"{}\", but it is not found in the shader.", material.getName(), textureName);
    }
  }
}

void MaterialSystem::allocateBuffers(Material& material, const izz::geo::MaterialTemplate& materialTemplate) {
  static std::unordered_map<izz::geo::PropertyType, uint64_t> PropertyTypeSize = {
      {izz::geo::PropertyType::BOOL, sizeof(GLint)},         {izz::geo::PropertyType::INT, sizeof(GLint)},
      {izz::geo::PropertyType::FLOAT, sizeof(GLfloat)},      {izz::geo::PropertyType::FLOAT4, 4 * sizeof(GLfloat)},
      {izz::geo::PropertyType::FLOAT3, 3 * sizeof(GLfloat)}, {izz::geo::PropertyType::FLOAT_ARRAY, sizeof(GLfloat)},
  };

  // allocate texture buffers based on texture descriptions.
  allocateTextureBuffers(material, materialTemplate.textures);

  for (auto& [name, uboDescription] : materialTemplate.uniformBuffers) {
    spdlog::debug("MaterialSystem: allocating uniform buffer on GPU");
    material.uniformBuffers[name] = createUniformBuffer(uboDescription, material);
  }

  // INIT UNSCOPED UNIFORMS
  //  first memory allocation. For this we need to know the number of properties and length of data properties.
  uint64_t globalUniformSize = 0U;
  uint64_t scopedUniformSize = 0U;
  int globalUniformCount = 0;
  int scopedUniformCount = 0;

  for (const auto& [name, uniform] : materialTemplate.uniforms) {
    if (PropertyTypeSize.count(uniform.type) == 0) {
      spdlog::warn("Property {} ignored", uniform.name);
      continue;
    }

    bool isGlobalUniform = (name.find(".") == std::string::npos);
    if (isGlobalUniform) {
      globalUniformCount++;
      globalUniformSize += PropertyTypeSize.at(uniform.type) * uniform.length;
    } else {
      scopedUniformCount++;
      scopedUniformSize += PropertyTypeSize.at(uniform.type) * uniform.length;
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
  material.globalUniforms = std::make_shared<UniformProperties>(globalUniformSize, globalUniformCount);
  material.scopedUniforms = std::make_shared<UniformProperties>(scopedUniformSize, scopedUniformCount);

  // TODO: useProgram

  for (const auto& [name, p] : materialTemplate.uniforms) {
    // global uniforms should not contain a "."
    int dotPosition = name.find(".");
    bool isGlobalUniform = (dotPosition == std::string::npos);
    auto& uniforms = isGlobalUniform ? material.globalUniforms : material.scopedUniforms;
    auto location = getUniformLocation(material.programId, name.c_str(), material.getName());
    UniformProperty* prop;

    switch (p.type) {
      case geo::PropertyType::BOOL:
        prop = uniforms->addBoolean(name, std::get<bool>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<int>(p.value));
        break;

      case geo::PropertyType::INT:
        prop = uniforms->addInt(name, std::get<int>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<int>(p.value));
        break;

      case geo::PropertyType::FLOAT:
        prop = uniforms->addFloat(name, std::get<float>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<float>(p.value));
        break;

      case geo::PropertyType::FLOAT_ARRAY: {
        prop = uniforms->addFloatArray(name, std::get<std::vector<float>>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        break;
      }

      default:
        spdlog::warn("{}: could not set uniform property '{}'. Data type not supported.", ID, p.name);
        break;
    }
  }
}

Material& MaterialSystem::createMaterial(const izz::geo::MaterialTemplate& materialTemplate) {
  return createMaterial(materialTemplate.name);
}

Material& MaterialSystem::createMaterial(std::string meshMaterialName, std::string instanceName) {
  const auto& materialTemplate = getMaterialTemplate(meshMaterialName);

  // 1. Check material name for collisions.
  if (!instanceName.empty() && m_createdMaterialNames.count(instanceName) > 0) {
    throw std::runtime_error("Cannot create material with name that already exists.");
  }

  // 2. If shader is not compiled, then compile it.
  if (!m_compiledMaterialTemplates.contains(materialTemplate.name)) {
    ShaderVariantInfo info = compileShader(materialTemplate);
    m_compiledShaderPrograms[info.programId] = info;
    m_compiledMaterialTemplates[materialTemplate.name] = info.programId;

    spdlog::debug("Compiled new program id {} from template {}", info.programId, materialTemplate.name);
  }

  // 3. Get a reference to the compiled shader info object.
  auto& shaderInfo = m_compiledShaderPrograms.at(m_compiledMaterialTemplates.at(materialTemplate.name));
  shaderInfo.numberOfInstances++;

  // 3. Create material and assign the material name.
  Material material;
  material.id = static_cast<MaterialId>(m_createdMaterials.size() + 1);
  material.programId = shaderInfo.programId;
  material.name = instanceName;

  // 4. Generate a unique material name (if no material name is specified).
  if (material.name.empty()) {
    material.name = materialTemplate.name + "_" + std::to_string(shaderInfo.numberOfInstances-1);
  }

  // 5. Allocate unique buffers (i.e. texture buffers, uniform buffers, etc.).
  allocateBuffers(material, materialTemplate);

  // 6. Bookkeeping: register the material's id and material's name.
  m_createdMaterials[material.id] = material;
  m_createdMaterialNames[material.name] = material.id;

  return m_createdMaterials.at(material.id);
}

const geo::MaterialTemplate& MaterialSystem::getMaterialTemplate(std::string meshMaterialName) {
  // 1. Map the mesh material name (if available in the mapping table).
  auto templateName = m_materialMappings.count(meshMaterialName) > 0 ? m_materialMappings.at(meshMaterialName) : meshMaterialName;

  if (!m_materialTemplates.contains(templateName)) {
    if (!m_materialTemplates.contains(m_defaultMaterialTemplateName)) {
      throw std::runtime_error(fmt::format("Failed to create material '{}': no such material is defined.", templateName));
    }

    templateName = m_defaultMaterialTemplateName;
  }

  return m_materialTemplates.at(templateName);
}

izz::gl::Material& MaterialSystem::makeDefaultMaterial() {
  return createMaterial(m_defaultMaterialTemplateName);
}

const std::unordered_map<int, Material>& MaterialSystem::getCreatedMaterials() {
  return m_createdMaterials;
}

const ShaderVariantInfo& MaterialSystem::getShaderVariantInfo(const Material& material) const {
  return m_compiledShaderPrograms.at(material.programId);
}

const std::unordered_map<int, ShaderVariantInfo>& MaterialSystem::getShaderPrograms() const{
  return m_compiledShaderPrograms;
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
