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
}


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

  UniformBuffer ub {};
  ub.data = m_uniformBlockManagers.at(bufferDescription.name)->CreateUniformBlock(ub.size);

  glGenBuffers(1, &ub.bufferId);
  glBindBuffer(GL_UNIFORM_BUFFER, ub.bufferId);

  glUseProgram(m.programId); // might not be needed
  ub.blockIndex = glGetUniformBlockIndex(m.programId, bufferDescription.name.c_str());
  if (ub.blockIndex == GL_INVALID_INDEX) {
    throw std::runtime_error(fmt::format("Could not find UBO with name {}", bufferDescription.name));
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


//void addTextureToMaterial(const std::string& textureName, const std::string& path, izz::gl::Material& material) {
//  if (textureName == ReservedNames::DIFFUSE_TEXTURE) {
//    material.diffuseTexturePath = path;
//  } else if (textureName == ReservedNames::SPECULAR_TEXTURE) {
//    material.specularTexturePath = path;
//  } else if (textureName == ReservedNames::NORMAL_TEXTURE) {
//    material.normalTexturePath = path;
//  } else if (textureName == ReservedNames::ROUGHNESS_TEXTURE) {
//    material.roughnessTexturePath = path;
//  } else {
//    material.texturePaths[textureName] = path;
//  }
//}

izz::geo::MaterialDescription& MaterialSystem::getMaterialDescription(const std::string& name) {
  return m_materialDescriptions.at(name);
}


bool MaterialSystem::hasMaterialDescription(const std::string& materialName) {
  return m_materialMappings.count(materialName) > 0 || m_materialDescriptions.count(materialName) > 0;
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
  spdlog::debug("\tProgram id: {}\n\tShader program compiled successfully (vs: {} fs: {})", material.programId,
                materialDescription.vertexShader, materialDescription.fragmentShader);
}

void MaterialSystem::allocateBuffers(Material& material, const izz::geo::MaterialDescription& materialDescription) {

  // load textures
  for (const auto& [textureName, texture] : materialDescription.textures) {
    //    for (const auto& [texname, filepath] : material.texturePaths) {
    spdlog::debug("Loading Texture {}: {}", textureName, texture.path.string());

    Texture* pTexture {nullptr};
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

  // load already loaded textures
//  for (auto& [name, textureBuffer] : material.textures) {
//    textureBuffer.location = glGetUniformLocation(material.programId, name.c_str());
//  }

  for (auto& [name, uboDescription] : materialDescription.uniformBuffers) {
    spdlog::debug("MaterialSystem: allocating uniform buffer on GPU");
    material.uniformBuffers[name] = createUniformBuffer(uboDescription, material);
  }

  // INIT UNSCOPED UNIFORMS
  //  first memory allocation. For this we need to know the number of properties and length of data properties.
  uint64_t sizeBytes = 0U;
  int numProperties = 0;
  for (const auto& [name, uniform] : materialDescription.uniforms) {
    switch(uniform.type) {
      case izz::geo::PropertyType::FLOAT:
      case izz::geo::PropertyType::INT:
      case izz::geo::PropertyType::BOOL:
        ++numProperties;
        sizeBytes += sizeof(GLint);
        break;

      case izz::geo::PropertyType::FLOAT3:
        ++numProperties;
        sizeBytes += 3*sizeof(GLfloat);
        break;

      case izz::geo::PropertyType::FLOAT4:
        ++numProperties;
        sizeBytes += 4*sizeof(GLfloat);
        break;

      case izz::geo::PropertyType::FLOAT_ARRAY:
        ++numProperties;
        sizeBytes += uniform.length*sizeof(GLfloat);
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

  material.unscopedUniformBuffer = UnscopedUniforms::Allocate(numProperties, sizeBytes);

  auto pData = material.unscopedUniformBuffer.pData;
  auto pUniform = material.unscopedUniformBuffer.pProperties;
  unsigned int offset = 0U;

//  for (auto [name, value] : materialDescription.uniforms) {
//    value.
//  }

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
