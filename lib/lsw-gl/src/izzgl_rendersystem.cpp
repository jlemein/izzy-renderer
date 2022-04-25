//
// Created by jlemein on 07-11-20.
//
#include <izzgl_rendersystem.h>
#include "gl_common.cpp"

#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <ecs_wireframe.h>
#include <geo_curve.h>
#include <geo_effect.h>
#include <geo_mesh.h>
#include <gl_renderutils.h>
#include <glrs_lightsystem.h>
#include <izz_resourcemanager.h>
#include <izzgl_effectsystem.h>
#include <izzgl_materialsystem.h>
#include <izzgl_shadersystem.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include "izzgl_material.h"
#include "izzgl_texturesystem.h"
#include "uniform_ubermaterial.h"

#include "izz_scenegraphhelper.h"
using namespace izz;
using namespace izz::gl;

namespace {

int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
  int location = glGetUniformLocation(program, name);
  if (location == -1) {
    throw std::runtime_error(fmt::format("unknown uniform parameter '{}' (material: '{}').", name, materialName));
  }
  return location;
}

//void pushShaderProperties(const Renderable&);
// void pushUnscopedUniforms(const Renderable&);

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

// void pushUnscopedUniforms(const RenderState& r) {
//   for (unsigned int i = 0; i < r.unscopedUniforms.numProperties; ++i) {
//     const auto& uniform = r.unscopedUniforms.pProperties[i];
//
//     switch (uniform.type) {
//       case UType::BOOL:
//       case UType::INT:
//         glUniform1i(uniform.location, *reinterpret_cast<int*>(r.unscopedUniforms.pData + uniform.offset));
//         break;
//
//       case UType::FLOAT_ARRAY:
//       case UType::FLOAT2:
//       case UType::FLOAT3:
//       case UType::FLOAT4:
//         glUniform1fv(uniform.location, uniform.length, reinterpret_cast<float*>(r.unscopedUniforms.pData + uniform.offset));
//         break;
//
//       case UType::INT_ARRAY:
//       case UType::INT2:
//       case UType::INT3:
//       case UType::INT4:
//         glUniform1iv(uniform.location, uniform.length, reinterpret_cast<GLint*>(r.unscopedUniforms.pData + uniform.offset));
//         break;
//     }
//   }
// }
//
//void initMVPUniformBlock(RenderState& renderable) {
//  glUseProgram(renderable.program);  // TODO: remove line
//  renderable.uboBlockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
//
//  if (renderable.uboBlockIndex == GL_INVALID_INDEX) {
//    //    throw std::runtime_error(fmt::format("Shader program does not contain a uniform block with name 'UniformBufferBlock' in {}",
//    //                                         renderable.material ? renderable.material->vertexShader : "<no material assigned>"));
//    throw std::runtime_error("Could not find UniformBufferBlock");
//  }
//
//  renderable.isMvpSupported = true;
//
//  glGenBuffers(1, &renderable.uboId);
//  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
////  GLint blockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
////  if (blockIndex == GL_INVALID_INDEX) {
////    std::cerr << "Cannot find ubo block with name 'UniformBufferBlock' in shader";
////    exit(1);
////  }
//
//  glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &renderable.uboBlockBinding);
//
//  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex, renderable.uboId);
//  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr, GL_DYNAMIC_DRAW);
//}
}  // namespace

// void RenderSystem::pushShaderProperties(const RenderState& r) {
//   for (const auto& [name, uniformBlock] : r.userProperties) {
//     glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
//     glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockBinding, uniformBlock.bufferId);
//
//     // is this needed?
//     glUniformBlockBinding(r.program, uniformBlock.blockIndex, uniformBlock.blockBinding);
//
//     void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
//     std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
//     glUnmapBuffer(GL_UNIFORM_BUFFER);
//   }
//
//   pushUnscopedUniforms(r);
// }

// void RenderSystem::pushLightingData(const RenderState& renderable) {
//   glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
//   glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);
//
//   // is this needed?
//   glUniformBlockBinding(renderable.program, renderable.uboLightingIndex, renderable.uboLightingBinding);
//
//   void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
//   std::memcpy(buff_ptr, renderable.pUboLightStruct, renderable.pUboLightStructSize);
//   glUnmapBuffer(GL_UNIFORM_BUFFER);
// }

// void RenderSystem::pushModelViewProjection(const RenderState& renderable) {
//   glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
//   glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockBinding, renderable.uboId);
//
//   // is this needed?
//   glUniformBlockBinding(renderable.program, renderable.uboBlockIndex, renderable.uboBlockBinding);
//
//   void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
//   UniformBlock b = renderable.uniformBlock;
//   std::memcpy(buff_ptr, &b, sizeof(UniformBlock));
//   glUnmapBuffer(GL_UNIFORM_BUFFER);
// }

LightSystem& RenderSystem::getLightSystem() {
  return *m_lightSystem;
}
//
//void RenderSystem::initShaderProperties(entt::entity entity, Renderable& renderable, const izz::gl::Material& material) {
//  glUseProgram(renderable.renderState.program);
//
//  if (!m_registry.all_of<Posteffect>(entity)) {
//    initMVPUniformBlock(renderable.renderState);
//  }
//
//  m_lightSystem->initLightingUbo(renderable.renderState, material);
//
//  // every shader usually has custom attributes
//  // they are processed here
//  for (const auto& [name, blockData] : material.properties) {
//    GLuint uboHandle;
//    glGenBuffers(1, &uboHandle);
//    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//
//    GLint blockIndex = glGetUniformBlockIndex(renderable.renderState.program, name.c_str());
//    GLint blockBinding;
//    glGetActiveUniformBlockiv(renderable.renderState.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
//
//    // is this needed?
//    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);
//
//    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);
//
//    if (blockIndex == GL_INVALID_INDEX) {
//      auto a = glGetUniformLocation(renderable.renderState.program, name.c_str());
//      throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, material.name));
//    }
//    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);
//
//    // store block handle in renderable
//    renderable.userProperties[name] = Renderable_UniformBlockInfo{uboHandle, blockIndex, blockBinding, &blockData};
//  }
//
//  initUnscopedShaderProperties(entity, renderable, material);
//}

RenderState& RenderSystem::createRenderState() {
  RenderState rs;
  rs.id = m_renderStates.size();
  m_renderStates.push_back(rs);
  return m_renderStates[rs.id];
}

const RenderState& RenderSystem::getRenderState(unsigned int id) const {
  try {
    return m_renderStates.at(id);
  } catch (std::out_of_range e) {
    throw std::runtime_error(fmt::format("Could not obtain render state with id {}", id));
  }
}

RenderState& RenderSystem::getRenderState(unsigned int id) {
  try {
    return m_renderStates.at(id);
  } catch (std::out_of_range e) {
    throw std::runtime_error(fmt::format("Could not obtain render state with id {}", id));
  }
}
//
//void RenderSystem::initUnscopedShaderProperties(entt::entity entity, Renderable& renderable, const izz::gl::Material& material) {
//  // first memory allocation. For this we need to know the number of properties and length of data properties.
//  int numProperties = material.unscopedUniforms.booleanValues.size() + material.unscopedUniforms.intValues.size() +
//                      material.unscopedUniforms.floatValues.size() + material.unscopedUniforms.floatArrayValues.size();
//  uint64_t sizeBytes = 0U;
//  sizeBytes += material.unscopedUniforms.booleanValues.size() * sizeof(GLint);
//  sizeBytes += material.unscopedUniforms.intValues.size() * sizeof(GLint);
//  sizeBytes += material.unscopedUniforms.floatValues.size() * sizeof(GLfloat);
//  for (const auto& array : material.unscopedUniforms.floatArrayValues) {
//    sizeBytes += array.second.size() * sizeof(GLfloat);
//  }
//
//  renderable.unscopedUniforms = UnscopedUniforms::Allocate(numProperties, sizeBytes);
//
//  auto pData = renderable.unscopedUniforms.pData;
//  auto pUniform = renderable.unscopedUniforms.pProperties;
//  unsigned int offset = 0U;
//
//  for (auto [name, value] : material.unscopedUniforms.booleanValues) {
//    *reinterpret_cast<int*>(pData) = value;
//    pUniform->location = getUniformLocation(renderable.renderState.program, name.c_str(), material.name);
//    pUniform->type = UType::BOOL;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1i(pUniform->location, *reinterpret_cast<int*>(renderable.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(GLint);
//    pData += sizeof(GLint);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.intValues) {
//    *reinterpret_cast<int*>(pData) = value;
//    pUniform->location = getUniformLocation(renderable.renderState.program, name.c_str(), material.name);
//    pUniform->type = UType::INT;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1i(pUniform->location, *reinterpret_cast<int*>(renderable.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(GLint);
//    pData += sizeof(GLint);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.floatValues) {
//    *reinterpret_cast<float*>(pData) = value;
//    pUniform->location = getUniformLocation(renderable.renderState.program, name.c_str(), material.name);
//    pUniform->type = UType::FLOAT;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1f(pUniform->location, *reinterpret_cast<float*>(renderable.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(float);
//    pData += sizeof(float);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
//    memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
//    pUniform->location = getUniformLocation(renderable.renderState.program, name.c_str(), material.name);
//    pUniform->type = UType::FLOAT_ARRAY;
//    pUniform->offset = offset;
//    pUniform->length = value.size();
//
//    glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(renderable.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));
//
//    offset += sizeof(float) * value.size();
//    pData += sizeof(float) * value.size();
//    pUniform++;
//  }
//}

RenderSystem::RenderSystem(entt::registry& registry, std::shared_ptr<lsw::ResourceManager> resourceManager, std::shared_ptr<MaterialSystem> materialSystem)
  : m_registry{registry}
//  , m_debugSystem(registry)
  , m_resourceManager{resourceManager}
  , m_materialSystem(materialSystem)
//  , m_effectSystem{effectSystem}
  , m_shaderSystem(std::make_shared<ShaderSystem>())
  , m_lightSystem{std::make_shared<LightSystem>(m_registry, materialSystem)}
//  , m_framebuffer{std::make_unique<HdrFramebuffer>()}
  , m_forwardRenderer(*this)
  , m_deferredRenderer(*this, registry) {}

void RenderSystem::initPostprocessBuffers() {
  // prepare quad for collecting
  float vertices[] = {
      // pos        // tex
      -1.0f, -1.f, 0.0f, 0.0f, 1.f, 1.f, 1.0f, 1.0f, -1.f, 1.f, 0.0f, 1.0f, -1.f, -1.f, 0.0f, 0.0f, 1.f, -1.f, 1.0f, 0.0f, 1.f, 1.f, 1.0f, 1.0f};
  // vao 1
  glGenVertexArrays(1, &m_quadVao);
  glGenBuffers(1, &m_quadVbo);
  glBindVertexArray(m_quadVao);
  glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + 4, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), izz::gl::BUFFER_OFFSET(2 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
//
//void RenderSystem::allocateTextureBuffers() {
//    const auto& textures = m_resourceManager->getTextureSystem()->getTextures();
//
//  for(auto& [name, texture] : textures) {
//    spdlog::debug("Rendersystem: allocating texture buffer for '{}'", name);
//    m_allocatedTextures[texture.id] = allocateTextureBuffer(texture);
//  }
//
////                                                      // <Material, Renderable>?
////
////  // put the code in bottom part of readMaterialDefinitions here
////  // ...
////
////
////
////    for (auto& [name, path] : geoMaterial.texturePaths) {
////      // TODO: check if name is used in shader object
////      try {
////        auto t = m_resourceManager->getTextureSystem()->loadTexture(path);
////        if (t != nullptr) {
////          geoMaterial.textures[name] = t->id;
////          attachTexture(renderable, *t, name);
////        }
////      } catch (std::out_of_range e) {
////        throw std::runtime_error(fmt::format("Cannot find texture with name \"{}\" in material \"{}\"", name, geoMaterial.name));
////      }
////    }
////
////    // A material is parsed in two ways
////    // - A material has standard textures assigned in the scene file, such as
////    //   diffuse, normal, transparency maps. They map to predefined variable
////    //   names.
////    // - A material requires a specific texture, such as noise, or any other
////    // texture of specific use for this material only.
////    //   In that case,
////    // diffuse texture
////    if (geoMaterial.diffuseTexture != nullptr) {
////      attachTexture(renderable, *geoMaterial.diffuseTexture, "diffuseTex");
////    }
////    if (geoMaterial.specularTexture != nullptr) {
////      attachTexture(renderable, *geoMaterial.specularTexture, "specularTex");
////    }
////    if (geoMaterial.normalTexture != nullptr) {
////      attachTexture(renderable, *geoMaterial.normalTexture, "normalTex");
////    }
////    if (geoMaterial.roughnessTexture != nullptr) {
////      attachTexture(renderable, *geoMaterial.roughnessTexture, "normalTex");
////    }
////  }
//}

void RenderSystem::init(int width, int height) {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(0.15F, 0.15F, 0.25F, 0.0F);

//  m_framebuffer->initialize();
  m_lightSystem->initialize();
//  m_effectSystem->initialize();

  // convert material descriptions to openGL specific material data.
//  allocateTextureBuffers();
//  m_materialSystem->synchronizeTextures(*this);

  //  m_effectSystem->initialize();

  // setup postprocessing screen quad
  initPostprocessBuffers();

  m_deferredRenderer.init(width, height);

  auto numMaterials = m_registry.view<Material>().size();
  auto numLights = m_lightSystem->getActiveLightCount();

  auto numDeferredRenderables = m_registry.view<gl::DeferredRenderable>().size();
  auto numForwardRenderables = m_registry.view<gl::Renderable>().size();
  spdlog::info("Forward renderables: {}, Deferred renderables: {}", numForwardRenderables, numDeferredRenderables);

  // small summary
  spdlog::info(
      "Render system initialized | "
      "Number of material in use: {} | "
      "Number of active lights: {}",
      numMaterials, numLights);
  return;

  // Initialization of the rendersystem encompasses the following steps.
  // Take into account the vocabulary.
  // * init: sets up the buffers, do precomputations and store parameters in
  // renderable component.
  // * update: reflect or synchronize the state of the system in the renderable
  // component.
  // * push: Reflect the changes to the GPU.
  //
  // * Synchronize the model view projection matrices into the renderable
  // component.
  // * Update the renderable component: allocate buffers and load mesh data in
  // GPU
  // * Push the remaining uniform parameter values to the shader.

  // handling curves
//  for (auto [entity, curve, renderable] : m_registry.view<lsw::geo::Curve, Renderable>().each()) {
//    try {
//      RenderUtils::FillBufferedMeshData(curve, renderable.renderState.meshData);
//    } catch (std::exception& e) {
//      auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
//
//      std::cerr << "Failed initializing curve '" << name << "': " << e.what();
//      exit(1);
//    }
//  }
//
//  // handling meshes
//  for (auto [entity, mesh, renderable] : m_registry.view<lsw::geo::Mesh, Renderable>().each()) {
//    auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
//
//    try {
//      RenderUtils::FillBufferedMeshData(mesh, renderable.renderState.meshData);
//
//    } catch (std::exception& e) {
//      auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
//      throw std::runtime_error(fmt::format("Failed initializing mesh '{}': {}", name, e.what()));
//    }
//  }
//
//  // handling materials
//  for (auto [entity, material, renderable] : m_registry.view<Material, Renderable>().each()) {
//    try {
//      if (material.isBinaryShader) {
//        renderable.renderState.program = m_shaderSystem->compileSpirvShader(material.vertexShader, material.fragmentShader);
//      } else {
//        renderable.renderState.program = m_shaderSystem->compileShader(material.vertexShader, material.fragmentShader);
//      }
//
//      spdlog::debug("#{} Shader program compiled successfully (vs: {} fs: {})", renderable.renderState.program, material.vertexShader,
//                    material.fragmentShader);
//
//      initShaderProperties(entity, renderable, material);
//
//    } catch (std::exception& e) {
//      auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
//      throw std::runtime_error(fmt::format("Entity {}: {}", name, e.what()));
//    }
//  }
  //  auto postprocessEffects = m_registry.view<ecs::Camera, ecs::Posteffect, geo::Material>();
}

void RenderSystem::update(float time, float dt) {
  // synchronizes the transformation for the entity into the renderable
  // component.
//  synchMvpMatrices();

  m_deferredRenderer.update();
  m_lightSystem->updateLightProperties();
  m_materialSystem->update(time, dt);


  //  m_materialSystem->update(time, dt); --> not needed I think
}

//void RenderSystem::synchMvpMatrices() {
//  // Updates the
//  // Render system updates the model view projection matrix for each of the
//  // The camera
//  auto view = m_registry.view<Renderable>();
//  for (auto entity : view) {
//    auto& renderable = m_registry.get<Renderable>(entity);
//
//    if (renderable.isMvpSupported) {
//      updateModelMatrix(entity);
//      updateCamera(renderable);
//    }
//  }
//}

//void RenderSystem::updateModelMatrix(entt::entity e) {
//  auto& renderable = m_registry.get<Renderable>(e);
//
//  // if the transformation matrix exists, apply it, otherwise take identity
//  // matrix.
//  // TODO: enforce that all entities do have a transform.
//  auto transform = m_registry.try_get<ecs::Transform>(e);
//
//  renderable.uniformBlock.model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);
//}

void RenderSystem::setActiveCamera(entt::entity cameraEntity) {
  if (!m_registry.all_of<lsw::ecs::Camera>(cameraEntity)) {
    throw std::runtime_error("Only entities with a Camera component can be set as active camera");
  }
  m_activeCamera = cameraEntity;

  m_forwardRenderer.setActiveCamera(cameraEntity);
  m_deferredRenderer.setActiveCamera(cameraEntity);
}

void RenderSystem::updateCamera(Renderable& renderable) {
  if (m_activeCamera == entt::null) {
    throw std::runtime_error("No active camera in scene");
  }

  auto& transform = m_registry.get<lsw::ecs::Transform>(m_activeCamera);
  auto& activeCamera = m_registry.get<lsw::ecs::Camera>(m_activeCamera);

  renderable.uniformBlock.view = glm::inverse(transform.worldTransform);
  renderable.uniformBlock.proj = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
  renderable.uniformBlock.viewPos = glm::vec3(transform.worldTransform[3]);
}

void RenderSystem::activateEffect(entt::entity e) {
  //  if (m_registry.all_of<geo::cEffect>(e)) {
  //    const auto& cfx = m_registry.get<izz::geo::cEffect>(e);
  //    const izz::geo::Effect& fx = m_effectSystem.getById(cfx.name);
  //
  //    fx.graph.
  //  }
  //  activateTextures(e);
}

void RenderSystem::render() {
  //  // 1. Select buffer to render into
//  m_framebuffer->bind();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//  m_forwardRenderer.render(m_registry);
  m_deferredRenderer.render(m_registry);

//  renderPosteffects();

//  m_framebuffer->apply();
}
//
//void RenderSystem::renderPosteffects() {
//  // activate screen quad
//  glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//  glBindVertexArray(m_quadVao);
//
//  if (m_registry.all_of<lsw::ecs::PosteffectCollection>(m_activeCamera)) {
//    int passId = 0;
//    const auto& collection = m_registry.get<lsw::ecs::PosteffectCollection>(m_activeCamera);
//
//    for (auto e : collection.posteffects) {
//      const auto& refEffect = m_registry.get<izz::geo::cEffect>(e);
//      izz::geo::Effect& effect = m_effectSystem->getEffect(refEffect);
//
//      for (int i = 0; i < effect.graph.nodeSize(); ++i) {
//        //      for (int i = 0; i < effect.graph.nodes().size(); ++i) {
//        //        glUseProgram(effect.graph.nodes[i].material.programId);
//
//        // Fetch each node ordered by depth. So when looping through the nodes from 0 to number of nodes.
//        // Fetch the node with depth 0 first. These are the first passes, with no previous passes.
//        // Their incoming connections are always empty.
//        const auto& node = effect.graph.sortedByDepth(i);
//
//        // Loop through the incoming edges/connections.
//        // The incoming connections indicate that there is output from a previous pass that we need in the current pass.
//        for (int j = 0; j < node.connectionsIn.size(); ++j) {
//          auto incomingEdge = effect.graph.edges()[node.connectionsIn[j]];
//
//          // one previous pass might have multiple output buffers that need to be mapped to multiple texture units.
//          for (const auto& mapping : incomingEdge.metadata.bufferMappings) {
//            glActiveTexture(GL_TEXTURE0 + mapping.inTextureUnit);
//            glBindTexture(GL_TEXTURE_2D, mapping.textureBuffer);
//          }
//        }
//
//        // now the outputs of previous pass are mapped.
//        // map the dynamic textures (obtained via the material properties)
//        // TODO: make sure a renderable is found on this entity.
//        // activateTextures(node.data.textures);
//
//        //        glBindFramebuffer(GL_READ_FRAMEBUFFER, node.depth == 0 ? 0 : effect.graph.nodes()[passId - 1].material->fbo);
//        auto& material = m_materialSystem->getMaterialById(node.data.materialId);
//        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, material.fbo);
//
//        // setup textures for next call
//      }
//
//      auto& renderable = m_registry.get<Renderable>(e);
//
//      //      glBindFramebuffer(GL_FRAMEBUFFER, renderable.fbo);
//      //      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderable.effect->nodes[passId].material->fbo);
//
//      m_framebuffer->nextPass();
//      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//      glUseProgram(renderable.renderState.program);
//
//      RenderUtils::ActivateUniformProperties(renderable.renderState);
//
//      glDrawArrays(GL_TRIANGLES, 0, 6);
//      ++passId;
//    }
//  }
//
//  glBindVertexArray(0);
//}

// void RenderSystem::renderPosteffects() {
//   // activate screen quad
//   glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//   glBindVertexArray(m_quadVao);
//
//   if (m_registry.all_of<ecs::PosteffectCollection>(m_activeCamera)) {
//     int passId = 0;
//
//     for (auto e : m_registry.get<ecs::PosteffectCollection>(m_activeCamera).posteffects) {
//       auto& effect = m_registry.get<izz::geo::Effect>(e);
//       for (int i = 0; i < effect.graph.nodes().size(); ++i) {
//         //        glUseProgram(effect.graph.nodes[i].material.programId);
//
//         glBindFramebuffer(GL_READ_FRAMEBUFFER, passId == 0 ? 0 : effect.graph.nodes()[passId - 1].material->fbo);
//         glBindFramebuffer(GL_DRAW_FRAMEBUFFER, effect.graph.nodes()[passId].material->fbo);
//
//         // setup textures for next call
//       }
//
//       auto& renderable = m_registry.get<Renderable>(e);
//
//       //      glBindFramebuffer(GL_FRAMEBUFFER, renderable.fbo);
//       //      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderable.effect->nodes[passId].material->fbo);
//
//       m_framebuffer->nextPass();
//       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//       glUseProgram(renderable.program);
//       pushShaderProperties(renderable);
//
//       glDrawArrays(GL_TRIANGLES, 0, 6);
//       ++passId;
//     }
//   }
//
//   glBindVertexArray(0);
// }

//IFramebuffer& RenderSystem::getFramebuffer() {
//  return *m_framebuffer;
//}

void RenderSystem::resize(int width, int height) {
//  getFramebuffer().resize(width, height);

}

MaterialSystem& RenderSystem::getMaterialSystem() {
  return *m_materialSystem;
}

void RenderSystem::checkError(entt::entity e) {
  // get error message
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    auto name = m_registry.all_of<lsw::ecs::Name>(e) ? m_registry.get<lsw::ecs::Name>(e).name : std::string{"Unnamed"};
    std::cerr << " Render error occurred for " << name << ": " << err << std::endl;
  }
}
