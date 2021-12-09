//
// Created by jlemein on 07-11-20.
//
#include <glrs_common.h>
#include <glrs_rendersystem.h>

#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <ecs_wireframe.h>
#include <ecsg_scenegraph.h>
#include <geo_curve.h>
#include <geo_material.h>
#include <geo_mesh.h>
#include <glrs_lightsystem.h>
#include <glrs_shadersystem.h>
#include <spdlog/spdlog.h>
#include <uniform_ubermaterial.h>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

using namespace lsw;
using namespace lsw::glrs;

namespace {

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void pushShaderProperties(const Renderable& r) {
  for (const auto& [name, uniformBlock] : r.userProperties) {
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockBinding, uniformBlock.bufferId);

    // is this needed?
    glUniformBlockBinding(r.program, uniformBlock.blockIndex, uniformBlock.blockBinding);

    void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }
}

void pushModelViewProjection(const Renderable& renderable) {
  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockBinding, renderable.uboId);

  // is this needed?
  glUniformBlockBinding(renderable.program, renderable.uboBlockIndex, renderable.uboBlockBinding);

  void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  UniformBlock b = renderable.uniformBlock;
  std::memcpy(buff_ptr, &b, sizeof(UniformBlock));
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void pushLightingData(const Renderable& renderable) {
  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding, renderable.uboLightingId);

  // is this needed?
  glUniformBlockBinding(renderable.program, renderable.uboLightingIndex, renderable.uboLightingBinding);

  void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  std::memcpy(buff_ptr, renderable.pUboLightStruct, renderable.pUboLightStructSize);
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void initMVPUniformBlock(Renderable& renderable) {
  glUseProgram(renderable.program);  // TODO: remove line
  renderable.uboBlockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");

  if (renderable.uboBlockIndex == GL_INVALID_INDEX) {
    throw std::runtime_error(fmt::format("Shader program does not contain a uniform block with name 'UniformBufferBlock' in {}",
                                         renderable.material ? renderable.material->vertexShader : "<no material assigned>"));
  }

  renderable.isMvpSupported = true;

  glGenBuffers(1, &renderable.uboId);

  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
  GLint blockIndex = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
  if (blockIndex == GL_INVALID_INDEX) {
    std::cerr << "Cannot find ubo block with name 'UniformBufferBlock' in shader";
    exit(1);
  }

  glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &renderable.uboBlockBinding);

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex, renderable.uboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr, GL_DYNAMIC_DRAW);
}
}  // namespace

glrs::LightSystem& RenderSystem::getLightSystem() {
  return *m_lightSystem;
}

void RenderSystem::initShaderProperties(entt::entity entity, Renderable& renderable, const geo::Material& material) {
  glUseProgram(renderable.program);

  if (!m_registry.all_of<glrs::Posteffect>(entity)) {
    initMVPUniformBlock(renderable);
  }

  m_lightSystem->initLightingUbo(renderable, material);

  // every shader usually has custom attributes
  // they are processed here
  for (const auto& [name, blockData] : material.properties) {
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    GLint blockIndex = glGetUniformBlockIndex(renderable.program, name.c_str());
    GLint blockBinding;
    glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);

    // is this needed?
    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);

    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);

    if (blockIndex == GL_INVALID_INDEX) {
      auto a = glGetUniformLocation(renderable.program, name.c_str());
      throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, material.name));
    }
    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

    //    auto p = reinterpret_cast<ufm::UberMaterial*>(blockData.data);
    //    std::cout << "Ubermaterial: hasDiffuse " << std::boolalpha << p->hasDiffuseTex << std::endl;
    //    std::cout << "   --- Init " << renderable.name << ">" << name << "
    //    with "
    //              << p->diffuse.r << " " << p->diffuse.g << " " <<
    //              p->diffuse.b
    //              << std::endl;

    // store block handle in renderable
    renderable.userProperties[name] = Renderable_UniformBlockInfo{uboHandle, blockIndex, blockBinding, &blockData};
  }
}

RenderSystem::RenderSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<IMaterialSystem> materialSystem)
  : m_registry{sceneGraph->getRegistry()}
  , m_debugSystem(sceneGraph->getRegistry())
  , m_materialSystem(materialSystem)
  , m_shaderSystem(std::make_shared<ShaderSystem>())
  , m_lightSystem{std::make_shared<LightSystem>(m_registry)} {}

void initCurveBuffers(Renderable& renderable, const geo::Curve& curve) {
  glGenBuffers(1, &renderable.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, curve.vertices.size() * sizeof(float), curve.vertices.data(), GL_STATIC_DRAW);

  renderable.index_buffer = 0U;
  //  renderable.vertexAttribCount = 1;
  //  renderable.vertexAttribArray[0].size = 3U;
  //  renderable.vertexAttribArray[0].buffer_offset = 0U;
  renderable.drawElementCount = curve.vertices.size() / 3;
  renderable.primitiveType = GL_LINES;

  glGenVertexArrays(1, &renderable.vertex_array_object);
  glBindVertexArray(renderable.vertex_array_object);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  // disable the buffers to prevent accidental manipulation
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initMeshBuffers(Renderable& renderable, const geo::Mesh& mesh) {
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint tangentSize = mesh.tangents.size() * sizeof(float);
  GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;

  glGenBuffers(1, &renderable.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr,
               GL_STATIC_DRAW);  // allocate buffer data only
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize,
                  mesh.vertices.data());  // fill partial data - vertices
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize,
                  mesh.normals.data());  // fill partial data - normals
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize,
                  mesh.uvs.data());  // fill partial data - normals

  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize, tangentSize,
                  mesh.tangents.data());  // fill partial data - normals

  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize + tangentSize, bitangentSize,
                  mesh.bitangents.data());  // fill partial data - normals

  glGenBuffers(1, &renderable.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

  renderable.vertexAttribCount = 5;
  // position data
  renderable.vertexAttribArray[0].size = 3U;
  renderable.vertexAttribArray[0].buffer_offset = 0U;
  // normal data
  renderable.vertexAttribArray[1].size = 3U;
  renderable.vertexAttribArray[1].buffer_offset = vertexSize;
  // uv data
  renderable.vertexAttribArray[2].size = 2U;
  renderable.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
  // tangent data
  renderable.vertexAttribArray[3].size = 3U;
  renderable.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
  // bitangent data
  renderable.vertexAttribArray[4].size = 3U;
  renderable.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;

  renderable.drawElementCount = mesh.indices.size();
  renderable.primitiveType = GL_TRIANGLES;

  glGenVertexArrays(1, &renderable.vertex_array_object);
  glBindVertexArray(renderable.vertex_array_object);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize + tangentSize));
  glBindVertexArray(0);
}

void RenderSystem::init() {
  m_framebuffer.initialize();

  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(0.15F, 0.15F, 0.25F, 0.0F);

  // initialize light system first. Light system may add renderable and material component for light sources.
  m_lightSystem->initialize();

  // convert material descriptions to openGL specific material data.
  m_materialSystem->synchronizeTextures(*this);

  auto numMaterials = m_registry.view<geo::Material>().size();
  auto numLights = m_lightSystem->getActiveLightCount();

  // small summary
  spdlog::info(
      "Render system initialized | "
      "Number of material in use: {} | "
      "Number of active lights: {}",
      numMaterials, numLights);

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
  for (auto [entity, curve, renderable] : m_registry.view<geo::Curve, Renderable>().each()) {
    try {
      initCurveBuffers(renderable, curve);
    } catch (std::exception& e) {
      auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";

      std::cerr << "Failed initializing curve '" << name << "': " << e.what();
      exit(1);
    }
  }

  // handling meshes
  for (auto [entity, mesh, renderable] : m_registry.view<geo::Mesh, Renderable>().each()) {
    auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";

    try {
      auto& renderable = m_registry.get<Renderable>(entity);
      auto& mesh = m_registry.get<geo::Mesh>(entity);
      initMeshBuffers(renderable, mesh);

    } catch (std::exception& e) {
      auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";
      throw std::runtime_error(fmt::format("Failed initializing mesh '{}': {}", name, e.what()));
    }
  }

  // handling materials
  for (auto [entity, material, renderable] : m_registry.view<geo::Material, Renderable>().each()) {
    try {
      spdlog::debug("Compiling shaders -- vs: {} fs: {}", material.vertexShader, material.fragmentShader);

      if (material.isBinaryShader) {
        renderable.program = m_shaderSystem->compileSpirvShader(material.vertexShader, material.fragmentShader);
      } else {
        renderable.program = m_shaderSystem->compileShader(material.vertexShader, material.fragmentShader);
      }

      initShaderProperties(entity, renderable, material);

    } catch (std::exception& e) {
      auto name = m_registry.all_of<ecs::Name>(entity) ? m_registry.get<ecs::Name>(entity).name : "Unnamed";
      throw std::runtime_error(fmt::format("Failed initializing material for entity with name '{}': {}", name, e.what()));
    }
  }
  //  auto postprocessEffects = m_registry.view<ecs::Camera, ecs::Posteffect, geo::Material>();
}

void RenderSystem::update(float time, float dt) {
  // synchronizes the transformation for the entity into the renderable
  // component.
  synchMvpMatrices();
  m_lightSystem->updateLightProperties();
  m_materialSystem->update(time, dt);

  //  m_materialSystem->update(time, dt); --> not needed I think
}

void RenderSystem::synchMvpMatrices() {
  // Updates the
  // Render system updates the model view projection matrix for each of the
  // The camera
  auto view = m_registry.view<Renderable>();
  for (auto entity : view) {
    auto& renderable = m_registry.get<Renderable>(entity);

    if (renderable.isMvpSupported) {
      updateModelMatrix(entity);
      updateCamera(renderable);
    }
  }
}

void RenderSystem::updateModelMatrix(entt::entity e) {
  auto& renderable = m_registry.get<Renderable>(e);

  // if the transformation matrix exists, apply it, otherwise take identity
  // matrix.
  // TODO: enforce that all entities do have a transform.
  auto transform = m_registry.try_get<ecs::Transform>(e);

  renderable.uniformBlock.model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);
}

void RenderSystem::setActiveCamera(entt::entity cameraEntity) {
  if (!m_registry.all_of<ecs::Camera>(cameraEntity)) {
    throw std::runtime_error("Only entities with a Camera component can be set as active camera");
  }
  m_activeCamera = cameraEntity;
}

void RenderSystem::updateCamera(Renderable& renderable) {
  if (m_activeCamera == entt::null) {
    throw std::runtime_error("No active camera in scene");
  }

  auto& transform = m_registry.get<ecs::Transform>(m_activeCamera);
  auto& activeCamera = m_registry.get<ecs::Camera>(m_activeCamera);

  renderable.uniformBlock.view = glm::inverse(transform.worldTransform);
  renderable.uniformBlock.proj = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
  renderable.uniformBlock.viewPos = glm::vec3(transform.worldTransform[3]);
}

void RenderSystem::render() {
  //  // 1. Select buffer to render into
  m_framebuffer.bindFramebuffer();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render 3D scene
  for (const auto& [entity, renderable, transform] : m_registry.view<const Renderable, const ecs::Transform>().each()) {
    const auto& name = m_registry.get<ecs::Name>(entity);

    // activate shader program
    glUseProgram(renderable.program);

    // prepare the materials
    activateTextures(entity);

    // TODO: disable in release
    if (renderable.isWireframe || m_registry.any_of<ecs::Wireframe>(entity)) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // bind the vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);
    //    glBindVertexArray(renderable.vertex_array_object);

    for (unsigned int i = 0U; i < renderable.vertexAttribCount; ++i) {
      const VertexAttribArray& attrib = renderable.vertexAttribArray[i];
      // todo: use VAOs
      glEnableVertexAttribArray(i);
      glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
    }

    // TODO: check if shader is dirty
    //  reason: if we push properties every frame (Except for MVP), we might
    //  unnecessary spend time doing that while we can immediately just render.
    pushShaderProperties(renderable);
    if (renderable.isMvpSupported) {
      pushModelViewProjection(renderable);
    }

    // TODO this one needs to change per glUseProgram, which is the case right
    //  now. In future we might optimize changing of glUseProgram in that
    //  case, this function should be called once per set of glUseProgram.
    if (renderable.isLightingSupported) {
      pushLightingData(renderable);
    }

    if (renderable.primitiveType == GL_TRIANGLES) {
      glDrawElements(renderable.primitiveType, renderable.drawElementCount, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(renderable.primitiveType, 0, renderable.drawElementCount);
    }

    // handle debug
    checkError(entity);
  }

  // render post processing effects
  if (m_registry.all_of<ecs::PosteffectCollection>(m_activeCamera)) {
    const auto& posteffectCollection = m_registry.get<ecs::PosteffectCollection>(m_activeCamera);
    for (auto e : posteffectCollection.posteffects) {
      m_framebuffer.nextPass();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const auto& renderable = m_registry.get<Renderable>(e);

      glUseProgram(renderable.program);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }

  m_framebuffer.blitToDefaultFramebuffer();
}

MultipassFramebuffer& RenderSystem::getFramebuffer() {
  return m_framebuffer;
}

void RenderSystem::checkError(entt::entity e) {
  // get error message
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    auto name = m_registry.all_of<ecs::Name>(e) ? m_registry.get<ecs::Name>(e).name : std::string{"Unnamed"};
    std::cerr << " Render error occurred for " << name << ": " << err << std::endl;
  }
}

/**!
 * @brief Creates a texture buffer and sends the data over to the GPU. This
 * method therefore converts a texture resource to GPU ready representation.
 * @param renderable      [in/out] Reference to renderable
 * @param geoTexture
 * @param name
 *
 * TODO: think about passing a scene graph entity instead
 * @return
 */
void RenderSystem::attachTexture(Renderable& renderable, const geo::Texture& geoTexture, const std::string& name) {
  ecs::Texture texture{.name = name};
  glGenTextures(1, &texture.glTextureId);
  glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!geoTexture.data.empty()) {
    GLint texChannel = geoTexture.channels == 3 ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, geoTexture.width, geoTexture.height, 0, texChannel, GL_UNSIGNED_BYTE, geoTexture.data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    spdlog::error("Failed to create texture buffer for texture '{}'. Data is empty.", geoTexture.path);
    exit(1);
  }

  renderable.textures.emplace_back(texture);
  spdlog::log(spdlog::level::debug, "Loaded GL texture for texture '{}'", name);
}

void RenderSystem::activateTextures(entt::entity e) {
  //  for (auto entity : view) {
  //  auto& registry = m_sceneGraph->getRegistry();

  if (!m_registry.all_of<Renderable, geo::Material>(e)) {
    return;
  }

  const auto& renderable = m_registry.get<Renderable>(e);
  //  const auto& shader = m_registry.get<Shader>(e);

  for (int t = 0; t < renderable.textures.size(); ++t) {
    auto& texture = renderable.textures[t];

    glActiveTexture(GL_TEXTURE0 + t);
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

    GLint s_diffuseMap = glGetUniformLocation(renderable.program, texture.name.c_str());
    //    if (s_diffuseMap == -1) {
    //        //std::cout << "Cannot find diffuse texture" << std::endl;
    //        //exit(1);
    //    } else {
    glUniform1i(s_diffuseMap, t);
  }

  //    }
  //    glBindSampler(0, 0);
}