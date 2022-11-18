//
// Created by jlemein on 05-08-21.
//
#include <izzgl_materialsystem.h>

#include <izz_resourcemanager.h>
#include <izzgl_shadersystem.h>
#include <izzgl_textureloader.h>
#include <izzgl_texturesystem.h>
#include <uniform_constant.h>
#include <uniform_debugproperties.h>
#include <uniform_depthpeeling.h>
#include <uniform_lambert.h>
#include <uniform_parallax.h>
#include <uniform_ubermaterial.h>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include "izz_capabilities.h"
#include "izzgl_shadersource.h"
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
  , m_resourceManager{resourceManager}
  , m_shaderCompiler{std::make_shared<ShaderCompiler>()} {
  // for now register the uniform blocks
  // these are the registered uniform blocks that the shaders can make use of.
  m_uniformBuffers[izz::ufm::Lambert::BUFFER_NAME] = std::make_unique<izz::ufm::LambertUniformManager>();
  m_uniformBuffers[izz::ufm::Parallax::BUFFER_NAME] = std::make_unique<izz::ufm::ParallaxManager>();
  m_uniformBuffers[izz::ufm::Uber::BUFFER_NAME] = std::make_unique<izz::ufm::UberUniformManager>();
  m_uniformBuffers[izz::ufm::ConstantLight::BUFFER_NAME] = std::make_unique<izz::ufm::ConstantManager>();
  m_uniformBuffers[izz::ufm::AlbedoSpecularity::BUFFER_NAME] = std::make_unique<izz::ufm::AlbedoSpecularityManager>();
  m_uniformBuffers[izz::ufm::BlinnPhong::BUFFER_NAME] = std::make_unique<izz::ufm::BlinnPhongManager>();
  m_uniformBuffers[izz::ufm::BlinnPhongSimple::BUFFER_NAME] = std::make_unique<izz::ufm::BlinnPhongSimpleManager>();
  m_uniformBuffers[izz::ufm::ModelViewProjection::BUFFER_NAME] = std::make_unique<izz::ufm::MvpSharedUniformUpdater>(registry);
  m_uniformBuffers[izz::ufm::DeferredLighting::BUFFER_NAME] = std::make_unique<izz::ufm::DeferredLightingUniform>(registry);
  m_uniformBuffers[izz::ufm::ForwardLighting::BUFFER_NAME] = std::make_unique<izz::ufm::ForwardLightingUniform>(registry);
  m_uniformBuffers[izz::ufm::DepthPeeling::BUFFER_NAME] = std::make_unique<izz::ufm::DepthPeelingUniformBuffer>();
  m_uniformBuffers[izz::ufm::DebugProperties::BUFFER_NAME] = std::make_unique<izz::ufm::DebugPropertiesUniform>();
}

void MaterialSystem::init() {
  ShaderCompiler compiler;
}

void MaterialSystem::addMaterialTemplate(izz::MaterialTemplate materialTemplate) {
  m_materialTemplates[materialTemplate.name] = materialTemplate;
}

UniformBuffer MaterialSystem::createUniformBuffer(const izz::UniformBufferDescription& bufferDescription, const Material& material) {
  spdlog::debug("MaterialSystem: creating UBO '{}' for material {}", bufferDescription.name, material.getName());

  if (m_uniformBuffers.count(bufferDescription.name) == 0) {
    throw std::runtime_error(
        fmt::format("Material '{}' is defined with a property '{}', but there is no uniform buffer defined for this property in the material system.",
                    material.name, bufferDescription.name));
  }

  // mark the buffer as being in use. If it already exists, this function does not insert again.
  m_uniformBuffersInUse.insert(bufferDescription.name);

  UniformBuffer ub{};
  ub.data = m_uniformBuffers.at(bufferDescription.name)->allocate(ub.size);

  glGenBuffers(1, &ub.bufferId);
  glBindBuffer(GL_UNIFORM_BUFFER, ub.bufferId);

  material.useProgram();  // might not be needed
  ub.blockIndex = glGetUniformBlockIndex(material.programId, bufferDescription.name.c_str());
  if (ub.blockIndex == GL_INVALID_INDEX) {
    //    throw std::runtime_error(fmt::format("Could not find UBO with name {}", bufferDescription.name));
    spdlog::error(
        "Material '{}' declares usage of UBO '{}', but it's not defined in the shader! This might cause "
        "runtime glitches.",
        material.name, bufferDescription.name);
    return UniformBuffer{};
  }
  glGetActiveUniformBlockiv(material.programId, ub.blockIndex, GL_UNIFORM_BLOCK_BINDING, &ub.blockBind);

  glBindBufferBase(GL_UNIFORM_BUFFER, ub.blockBind, ub.bufferId);
  glBufferData(GL_UNIFORM_BUFFER, ub.size, NULL, GL_DYNAMIC_DRAW);

  return ub;
}

bool MaterialSystem::hasMaterialTemplate(const std::string& materialTemplateName) {
  return m_materialMappings.contains(materialTemplateName) || m_materialTemplates.contains(materialTemplateName);
}

void MaterialSystem::setDefaultMaterial(const std::string& name) {
  if (m_materialTemplates.count(name) > 0) {
    m_defaultMaterialTemplateName = name;
  } else {
    spdlog::warn("Setting default material '{}' failed, because material system failed to find a material description with that name.");
  }
}

void MaterialSystem::setShaderRootDirectory(std::filesystem::path shaderRoot) {
  m_shaderRootDirectory = shaderRoot;
}

ShaderVariantInfo MaterialSystem::compileShader(const izz::MaterialTemplate& materialTemplate) {
  ShaderVariantInfo info;

  ShaderContext context;
  context.root = m_shaderRootDirectory;

  // before shader is compiled. set the render capabilities.
  // Capabilities are coming from two sources:
  //   * defined by the user, i.e. compile time constants set in the materials.json file.
  //   * defined by the render capabilities (in practice usually decided by the render system).
  for (auto& flag : materialTemplate.compileConstants.flags) {
    context.compileConstants.flags.insert(flag);
    //    m_shaderCompiler->enableCompileConstant(flag);
  }

  if (materialTemplate.isBinaryShader) {
    info.programId = m_shaderCompiler->compileSpirvShader(materialTemplate.vertexShader, materialTemplate.fragmentShader);
  } else {
    info.programId = m_shaderCompiler->compileShader(materialTemplate.vertexShader, materialTemplate.fragmentShader, &context);
  }
  spdlog::debug("Compilation shader '{}' success.", materialTemplate.name);

  info.materialTemplateName = materialTemplate.name;
  return info;
}

void MaterialSystem::allocateTextureBuffers(Material& material, const std::unordered_map<std::string, izz::TextureDescription>& textureDescriptions) {
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

void MaterialSystem::allocateBuffers(Material& material, const MaterialTemplate& materialTemplate) {
  static std::unordered_map<izz::PropertyType, uint64_t> PropertyTypeSize = {
      {PropertyType::BOOL, sizeof(GLint)},         {PropertyType::INT, sizeof(GLint)},
      {PropertyType::FLOAT, sizeof(GLfloat)},      {PropertyType::FLOAT4, 4 * sizeof(GLfloat)},
      {PropertyType::FLOAT3, 3 * sizeof(GLfloat)}, {PropertyType::FLOAT_ARRAY, sizeof(GLfloat)},
      {PropertyType::INT_ARRAY, sizeof(GLint)}};

  // allocate texture buffers based on texture descriptions.
  allocateTextureBuffers(material, materialTemplate.textures);

  for (auto& [name, uboDescription] : materialTemplate.uniformBuffers) {
    spdlog::debug("MaterialSystem: allocating uniform buffer on GPU");
    if (m_uniformBuffers.contains(uboDescription.name)) {
      auto buffer = createUniformBuffer(uboDescription, material);
      if (buffer.data != nullptr) {
        material.uniformBuffers[name] = createUniformBuffer(uboDescription, material);
      }

      m_uniformBuffers.at(name)->onInit();
    }
  }

  // INIT UNSCOPED UNIFORMS
  //  first memory allocation. For this we need to know the number of properties and length of data properties.
  uint64_t globalUniformSize = 0U;
  uint64_t scopedUniformSize = 0U;
  int globalUniformCount = 0;
  int scopedUniformCount = 0;

  for (const auto& [name, uniform] : materialTemplate.uniforms) {
    if (PropertyTypeSize.count(uniform.type) == 0) {
      throw std::runtime_error(fmt::format("Data type of uniform property '{}' is unknown. Will result in memory corruption.", uniform.name));
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
      case PropertyType::BOOL:
        prop = uniforms->addBoolean(name, std::get<bool>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<bool>(p.value));
        break;

      case PropertyType::INT:
        prop = uniforms->addInt(name, std::get<int>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<int>(p.value));
        break;

      case PropertyType::FLOAT:
        prop = uniforms->addFloat(name, std::get<float>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        spdlog::info("Initialized: {}.{} = {}", material.getName(), name, std::get<float>(p.value));
        break;

      case PropertyType::FLOAT2:
      case PropertyType::FLOAT3:
      case PropertyType::FLOAT4:
      case PropertyType::FLOAT_ARRAY: {
        prop = uniforms->addFloatArray(name, std::get<std::vector<float>>(p.value), location);
        material.m_allUniforms[name] = prop;
        if (!isGlobalUniform) {
          material.m_allUniforms[name.substr(dotPosition + 1)] = prop;
        }
        break;
      }

      case PropertyType::INT2:
      case PropertyType::INT3:
      case PropertyType::INT4:
      case PropertyType::INT_ARRAY: {
        prop = uniforms->addIntArray(name, std::get<std::vector<int32_t>>(p.value), location);
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

Material& MaterialSystem::createMaterial(izz::MaterialTemplate materialTemplate, std::string name) {
  // 1. Check material name for collisions.
  if (!name.empty() && m_createdMaterialNames.count(name) > 0) {
    throw std::runtime_error("Cannot create material with name that already exists.");
  }

  // 2. Append render capabilities to material template
  auto renderCapabilities = m_capabilitySelector->selectRenderCapabilities(materialTemplate);
  materialTemplate.compileConstants += renderCapabilities;

  // 3. If shader variant is not compiled, then compile it.
  // Shader variant included compile time constants.
  auto shaderIdentifier = fmt::format("{}_{}", materialTemplate.name, std::hash<RenderCapabilities>{}(materialTemplate.compileConstants));

  if (!m_compiledMaterialTemplates.contains(shaderIdentifier)) {
    ShaderVariantInfo info = compileShader(materialTemplate);
    m_compiledShaderPrograms[info.programId] = info;
    m_compiledMaterialTemplates[shaderIdentifier] = info.programId;

    spdlog::debug("Compiled new program id {} from template {}", info.programId, shaderIdentifier);
  }

  // 4. Get a reference to the compiled shader info object.
  auto& shaderInfo = m_compiledShaderPrograms.at(m_compiledMaterialTemplates.at(shaderIdentifier));
  shaderInfo.numberOfInstances++;

  // 5. Create material and assign the material name.
  Material material;
  material.id = static_cast<MaterialId>(m_createdMaterials.size() + 1);
  material.programId = shaderInfo.programId;
  material.name = name;
  material.blendMode = materialTemplate.blendMode;

  // 6. Generate a unique material name (if no material name is specified).
  if (material.name.empty()) {
    material.name = materialTemplate.name;

    if (shaderInfo.numberOfInstances > 1) {  // don't append instance number if it's the first instance
      material.name += "_" + std::to_string(shaderInfo.numberOfInstances - 1);
    }
  }

  // 7. Allocate unique buffers (i.e. texture buffers, uniform buffers, etc.).
  allocateBuffers(material, materialTemplate);

  // 8. Bookkeeping: register the material's id and material's name.
  m_createdMaterials[material.id] = material;
  m_createdMaterialNames[material.name] = material.id;

  return m_createdMaterials.at(material.id);
}

Material& MaterialSystem::createMaterial(const std::string& meshMaterialName, std::string instanceName) {
  // copy material template, because compile constants may differ
  auto materialTemplate = getMaterialTemplate(meshMaterialName);

  // 1. Check material name for collisions.
  if (!instanceName.empty() && m_createdMaterialNames.count(instanceName) > 0) {
    throw std::runtime_error("Cannot create material with name that already exists.");
  }

  // 2. Append render capabilities to material template
  auto renderCapabilities = m_capabilitySelector->selectRenderCapabilities(materialTemplate);
  materialTemplate.compileConstants += renderCapabilities;

  // 3. If shader variant is not compiled, then compile it.
  // Shader variant included compile time constants.
  auto shaderIdentifier = fmt::format("{}_{}", materialTemplate.name, std::hash<RenderCapabilities>{}(materialTemplate.compileConstants));

  if (!m_compiledMaterialTemplates.contains(shaderIdentifier)) {
    ShaderVariantInfo info = compileShader(materialTemplate);
    m_compiledShaderPrograms[info.programId] = info;
    m_compiledMaterialTemplates[shaderIdentifier] = info.programId;

    spdlog::debug("Compiled new program id {} from template {}", info.programId, shaderIdentifier);
  }

  // 4. Get a reference to the compiled shader info object.
  auto& shaderInfo = m_compiledShaderPrograms.at(m_compiledMaterialTemplates.at(shaderIdentifier));
  shaderInfo.numberOfInstances++;

  // 5. Create material and assign the material name.
  Material material;
  material.id = static_cast<MaterialId>(m_createdMaterials.size() + 1);
  material.programId = shaderInfo.programId;
  material.name = instanceName;
  material.blendMode = materialTemplate.blendMode;

  // 6. Generate a unique material name (if no material name is specified).
  if (material.name.empty()) {
    material.name = materialTemplate.name;
    if (shaderInfo.numberOfInstances > 1) {  // don't append instance number if it's the first instance
      material.name += "_" + std::to_string(shaderInfo.numberOfInstances - 1);
    }
  }

  // 7. Allocate unique buffers (i.e. texture buffers, uniform buffers, etc.).
  allocateBuffers(material, materialTemplate);

  // 8. Bookkeeping: register the material's id and material's name.
  m_createdMaterials[material.id] = material;
  m_createdMaterialNames[material.name] = material.id;

  return m_createdMaterials.at(material.id);
}

const MaterialTemplate& MaterialSystem::getMaterialTemplate(std::string meshMaterialName) {
  // 1. Map the mesh material name (if available in the mapping table).
  auto templateName = meshMaterialName;
  if (m_materialMappings.count(meshMaterialName) > 0) {
    templateName = m_materialMappings.at(meshMaterialName);
    spdlog::debug("material mapping override: {} maps to material: {}", meshMaterialName, templateName);
  }

  if (!m_materialTemplates.contains(templateName)) {
    if (!m_materialTemplates.contains(m_defaultMaterialTemplateName)) {
      throw std::runtime_error(fmt::format("Failed to create material '{}': no such material is defined.", templateName));
    }

    spdlog::warn("Material with name '{}' is not defined. The default material '{}' is used.", meshMaterialName, m_defaultMaterialTemplateName);
    templateName = m_defaultMaterialTemplateName;
  }

  return m_materialTemplates.at(templateName);
}

const MaterialTemplate& MaterialSystem::getMaterialTemplateFromMaterial(int materialId) const {
  try {
    auto programId = m_createdMaterials.at(materialId).programId;
    ShaderVariantInfo info = m_compiledShaderPrograms.at(programId);
    return m_materialTemplates.at(info.materialTemplateName);
  } catch (std::out_of_range e) {
    throw std::runtime_error(fmt::format("Failed retrieving material template from specified material id {}", materialId));
  }
}

izz::gl::Material& MaterialSystem::makeDefaultMaterial() {
  return createMaterial(m_defaultMaterialTemplateName);
}

std::unordered_map<MaterialId, Material>& MaterialSystem::getCreatedMaterials() {
  return m_createdMaterials;
}

const std::unordered_map<MaterialId, Material>& MaterialSystem::getCreatedMaterials() const {
  return m_createdMaterials;
}

void MaterialSystem::setCapabilitySelector(const IRenderCapabilitySelector* selector) {
  m_capabilitySelector = selector;
}

const ShaderVariantInfo& MaterialSystem::getShaderVariantInfo(const Material& material) const {
  return m_compiledShaderPrograms.at(material.programId);
}

const std::unordered_map<int, ShaderVariantInfo>& MaterialSystem::getShaderPrograms() const {
  return m_compiledShaderPrograms;
}

izz::gl::Material& MaterialSystem::getMaterialById(int materialId) {
  try {
    return m_createdMaterials.at(materialId);
  } catch (std::out_of_range&) {
    throw std::runtime_error(fmt::format("Could not find material with id {}", materialId));
  }
}

izz::gl::ShaderCompiler& MaterialSystem::getShaderCompiler() {
  return *m_shaderCompiler;
}

void MaterialSystem::update(float dt, float time) {
  for (auto& bufferName : m_uniformBuffersInUse) {
    m_uniformBuffers.at(bufferName)->onFrameStart(dt, time);
  }

  for (auto& [id, material] : m_createdMaterials) {
    for (const auto& [name, uniformBlock] : material.uniformBuffers) {
      m_uniformBuffers.at(name)->onUpdate(uniformBlock.data, material, dt, time);
    }
  }
}

void MaterialSystem::updateUniformsForEntity(entt::entity e, Material& material) {
  for (auto uniformBuffer : material.uniformBuffers) {
    const auto& bufferName = uniformBuffer.first;
    m_uniformBuffers.at(bufferName)->onEntityUpdate(e, material);
  }
}