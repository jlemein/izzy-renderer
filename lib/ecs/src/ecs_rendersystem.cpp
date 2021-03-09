//
// Created by jlemein on 07-11-20.
//
#include <ecs_rendersystem.h>

#include <cstring>
#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_texturesystem.h>
#include <ecs_transform.h>
#include <ecsg_scenegraph.h>
#include <fstream>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_ubermaterialdata.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
using namespace affx;
using namespace affx::ecs;

namespace {
std::vector<char> readFile(const std::string &filename) {
  // TODO: check for empty file name and give clear error message

  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("failed to read file '{}'", filename));
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

void getShaderLog(GLint shader) {
  std::cout << "Failed to compile shader" << std::endl;
  GLint reqBufferSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &reqBufferSize);

  std::vector<char> buffer;
  buffer.resize(reqBufferSize);
  glGetShaderInfoLog(shader, reqBufferSize, nullptr, buffer.data());

  std::string log(buffer.begin(), buffer.end());
  std::cout << log << std::endl;
}

void printLinkingLog(GLint program) {
  spdlog::log(spdlog::level::err, "Failed to link shader");

  GLint reqBufferSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &reqBufferSize);

  std::vector<char> buffer;
  buffer.resize(reqBufferSize);
  glGetProgramInfoLog(program, reqBufferSize, nullptr, buffer.data());

  std::string log(buffer.begin(), buffer.end());
  spdlog::log(spdlog::level::info, "Program log: {}", log);
}

constexpr void *BUFFER_OFFSET(unsigned int offset) {
  uint8_t *pAddress = 0;
  return pAddress + offset;
}

void pushShaderProperties(const Renderable &r) {
  for (const auto &[name, uniformBlock] : r.userProperties) {

    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockBinding,
                     uniformBlock.bufferId);

    // is this needed?
    glUniformBlockBinding(r.program, uniformBlock.blockIndex,
                          uniformBlock.blockBinding);

    void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }
}

void pushModelViewProjection(const Renderable &renderable) {
  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockBinding,
                   renderable.uboId);

  // is this needed?
  glUniformBlockBinding(renderable.program, renderable.uboBlockIndex,
                        renderable.uboBlockBinding);

  void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  UniformBlock b = renderable.uniformBlock;
  std::memcpy(buff_ptr, &b, sizeof(UniformBlock));
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void pushLightingData(const Renderable &renderable,
                      const UniformLighting &lightingData) {
  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding,
                   renderable.uboLightingId);

  // is this needed?
  glUniformBlockBinding(renderable.program, renderable.uboLightingIndex,
                        renderable.uboLightingBinding);

  void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  std::memcpy(buff_ptr, &lightingData, sizeof(UniformLighting));
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void initMVPUniformBlock(Renderable &renderable) {
  glUseProgram(renderable.program); // TODO: remove line
  renderable.uboBlockIndex =
      glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
  if (renderable.uboBlockIndex == GL_INVALID_INDEX) {
    throw std::runtime_error("Invalid uniform buffer object block index");
  }

  glGenBuffers(1, &renderable.uboId);

  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboId);
  GLint blockIndex =
      glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
  if (blockIndex == GL_INVALID_INDEX) {
    std::cerr
        << "Cannot find ubo block with name 'UniformBufferBlock' in shader";
    exit(1);
  }

  glGetActiveUniformBlockiv(renderable.program, blockIndex,
                            GL_UNIFORM_BLOCK_BINDING,
                            &renderable.uboBlockBinding);

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex,
                   renderable.uboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr,
               GL_DYNAMIC_DRAW);
}

void initLightingUbo(Renderable &renderable) {
  renderable.uboLightingIndex =
      glGetUniformBlockIndex(renderable.program, UniformLighting::PARAM_NAME);
  if (renderable.uboLightingIndex == GL_INVALID_INDEX) {
    spdlog::debug("Lighting disabled, cannot find ubo block index with name {}",  UniformLighting::PARAM_NAME);
    renderable.isLightingSupported = false;
  } else {
    renderable.isLightingSupported = true;
  }

  glGenBuffers(1, &renderable.uboLightingId);

  glBindBuffer(GL_UNIFORM_BUFFER, renderable.uboLightingId);
  GLint blockIndex =
      glGetUniformBlockIndex(renderable.program, UniformLighting::PARAM_NAME);

  glGetActiveUniformBlockiv(renderable.program, blockIndex,
                            GL_UNIFORM_BLOCK_BINDING,
                            &renderable.uboLightingBinding);

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingBinding,
                   renderable.uboLightingId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformLighting), nullptr,
               GL_DYNAMIC_DRAW);
}

/**
 *
 * @param renderable
 * @param properties
 */
void initShaderProperties(Renderable &renderable,
                          const geo::Material::UniformProperties &properties) {
  glUseProgram(renderable.program);

  // allocates the MVP matrices on the GPU.
  initMVPUniformBlock(renderable);
  initLightingUbo(renderable);

  // every shader usually has custom attributes
  // they are processed here
  for (const auto &[name, blockData] : properties) {
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    GLint blockIndex = glGetUniformBlockIndex(renderable.program, name.c_str());
    GLint blockBinding;
    glGetActiveUniformBlockiv(renderable.program, blockIndex,
                              GL_UNIFORM_BLOCK_BINDING, &blockBinding);

    // is this needed?
    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);

    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);

    if (blockIndex == GL_INVALID_INDEX) {
      std::cerr << "Cannot find ubo block with name '" << name << "' in shader";
      exit(1);
    }
    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

    auto p = reinterpret_cast<geo::UberMaterialData *>(blockData.data);
    std::cout << "Ubermaterial: hasDiffuse " << std::boolalpha << p->hasDiffuseTex << std::endl;
    //    std::cout << "   --- Init " << renderable.name << ">" << name << "
    //    with "
    //              << p->diffuse.r << " " << p->diffuse.g << " " <<
    //              p->diffuse.b
    //              << std::endl;

    // store block handle in renderable
    renderable.userProperties[name] = Renderable_UniformBlockInfo{
        uboHandle, blockIndex, blockBinding, &blockData};
  }
}

} // namespace

RenderSystem::RenderSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph)
    : m_registry{sceneGraph->getRegistry()},
      m_debugSystem(sceneGraph->getRegistry()),
      m_materialSystem(std::make_shared<TextureSystem>(sceneGraph)) {}

void initCurveBuffers(Renderable &renderable, const geo::Curve &curve) {
  glGenBuffers(1, &renderable.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, curve.vertices.size() * sizeof(float),
               curve.vertices.data(), GL_STATIC_DRAW);

  renderable.index_buffer = 0U;

  renderable.vertexAttribCount = 1;
  renderable.vertexAttribArray[0].size = 3U;
  renderable.vertexAttribArray[0].buffer_offset = 0U;
  renderable.drawElementCount = curve.vertices.size() / 3;
  renderable.primitiveType = GL_LINES;

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void initMeshBuffers(Renderable &renderable, const geo::Mesh &mesh) {
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize;

  glGenBuffers(1, &renderable.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr,
               GL_STATIC_DRAW); // allocate buffer data only
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize,
                  mesh.vertices.data()); // fill partial data - vertices
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize,
                  mesh.normals.data()); // fill partial data - normals
  glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize,
                  mesh.uvs.data()); // fill partial data - normals

  glGenBuffers(1, &renderable.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t),
               mesh.indices.data(), GL_STATIC_DRAW);

  renderable.vertexAttribCount = 3;
  renderable.vertexAttribArray[0].size = 3U;
  renderable.vertexAttribArray[0].buffer_offset = 0U;
  renderable.vertexAttribArray[1].size = 3U;
  renderable.vertexAttribArray[1].buffer_offset = vertexSize;
  renderable.vertexAttribArray[2].size = 2U;
  renderable.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
  renderable.drawElementCount = mesh.indices.size();
  renderable.primitiveType = GL_TRIANGLES;

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
                        BUFFER_OFFSET(vertexSize + normalSize));
}
GLuint compileShader(const geo::Material &material, const Renderable &renderable) {
  auto vertexShaderBuffer = readFile(material.vertexShader);
  auto fragmentShaderBuffer = readFile(material.fragmentShader);

  int vsCompiled = 0, fsCompiled = 0;
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 vertexShaderBuffer.data(), vertexShaderBuffer.size());
  glSpecializeShaderARB(vertex_shader, "main", 0, 0, 0);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);

  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                 fragmentShaderBuffer.data(), fragmentShaderBuffer.size());
  glSpecializeShaderARB(fragment_shader, "main", 0, 0, 0);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsCompiled);

  if (!fsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile fragment shader");
    getShaderLog(fragment_shader);
  }

  if (!vsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile vertex shader");
    getShaderLog(vertex_shader);
  }

  if (!fsCompiled || !vsCompiled) {
    exit(1);
  }

  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  int linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);

  if (linked == GL_FALSE) {
    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}",
                material.vertexShader, material.fragmentShader);
    printLinkingLog(program);
  }

  return program;
}

void RenderSystem::init() {
  glShadeModel(GL_SMOOTH);

  // convert material descriptions to openGL specific material data.
  m_materialSystem->initialize();

  // small summary
  spdlog::info("Render system initialized\n"
               "Number of material in use {}\n"
               "Number of active lights: {}", "Unknown", m_registry.view<Light>().size());

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
  auto curveView = m_registry.view<geo::Curve, Renderable>();
  for (auto entity : curveView) {
    auto &curve = m_registry.get<geo::Curve>(entity);
    auto &renderable = m_registry.get<Renderable>(entity);
    auto &material = m_registry.get<geo::Material>(entity);

    try {
      initCurveBuffers(renderable, curve);
      renderable.program = compileShader(material, renderable);
      initShaderProperties(renderable, material.properties);
    } catch (std::exception &e) {
      auto name = m_registry.has<Name>(entity)
                      ? m_registry.get<Name>(entity).name
                      : "Unnamed";

      std::cerr << "Failed initializing curve '" << name << "': " << e.what();
      exit(1);
    }
  }

  // handling meshes
  auto view = m_registry.view<geo::Mesh, geo::Material, Renderable>();
  for (auto entity : view) {
    auto &mesh = m_registry.get<geo::Mesh>(entity);
    auto &renderable = m_registry.get<Renderable>(entity);
    auto &material = m_registry.get<geo::Material>(entity);

    auto name = m_registry.has<Name>(entity)
                ? m_registry.get<Name>(entity).name
                : "Unnamed";

    try {
      initMeshBuffers(renderable, mesh);
      renderable.program = compileShader(material, renderable);
      spdlog::info("Init shader properties for {}, mtl name: {} -> vs: {}", name, material.name, material.vertexShader);
      initShaderProperties(renderable, material.properties);

    } catch (std::exception &e) {
      auto name = m_registry.has<Name>(entity)
                      ? m_registry.get<Name>(entity).name
                      : "Unnamed";

      std::cerr << "Failed initializing mesh '" << name << "': " << e.what();
      exit(1);
    }
  }
}

void RenderSystem::update(float time, float dt) {
  // synchronizes the transformation for the entity into the renderable
  // component.
  synchMvpMatrices();
  updateLightProperties();
  m_materialSystem->update(time, dt);
}

void RenderSystem::synchMvpMatrices() {
  // Updates the
  // Render system updates the model view projection matrix for each of the
  // The camera
  auto view = m_registry.view<Renderable>();
  for (auto entity : view) {
    auto &renderable = m_registry.get<Renderable>(entity);

    updateModelMatrix(entity);
    updateCamera(renderable);
  }
}

void RenderSystem::updateLightProperties() {
  auto view = m_registry.view<Transform, Light>();
  m_uLightData.numberLights = view.size();

  if (view.size() == 0) {
    std::cout << "WARNING: No active lights in scene." << std::endl;
  }
  if (view.size() > 4) {
    throw std::runtime_error(
        "Too many lights in the scene. Max supported is 4");
  }

  unsigned int i = 0U;
  for (auto e : view) {
    const auto &light = view.get<Light>(e);
    const auto &transform = view.get<Transform>(e);
    const auto &name = m_registry.get<Name>(e);

    m_uLightData.diffuseColors[i] = glm::vec4(light.diffuseColor, 1.0F);
    m_uLightData.intensities[i] = light.intensity;
    m_uLightData.attenuation[i] = light.attenuationQuadratic;
    m_uLightData.positions[i] = transform.worldTransform[3];
    ++i;
  }
}

void RenderSystem::updateModelMatrix(entt::entity e) {
  auto &renderable = m_registry.get<Renderable>(e);

  // if the transformation matrix exists, apply it, otherwise take identity
  // matrix.
  // TODO: enforce that all entities do have a transform.
  auto transform = m_registry.try_get<ecs::Transform>(e);

  renderable.uniformBlock.model =
      transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);
}

void RenderSystem::setActiveCamera(entt::entity cameraEntity) {
  if (!m_registry.has<Camera>(cameraEntity)) {
    throw std::runtime_error(
        "Only entities with a Camera component can be set as active camera");
  }
  m_activeCamera = cameraEntity;

  // determine frame buffers
  if (m_registry.has<PosteffectCollection>(m_activeCamera)) {
    GLuint framebufferName = 0;
    glGenFramebuffers(1, &framebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);

    // texture we are going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthrenderbuffer);
  }
}

void RenderSystem::updateCamera(Renderable &renderable) {

  if (m_activeCamera == entt::null) {
    throw std::runtime_error("No active camera in scene");
  }

  auto &transform = m_registry.get<ecs::Transform>(m_activeCamera);
  auto &activeCamera = m_registry.get<Camera>(m_activeCamera);

  renderable.uniformBlock.view = glm::inverse(transform.worldTransform);
  renderable.uniformBlock.proj =
      glm::perspective(activeCamera.fovx, activeCamera.aspect,
                       activeCamera.zNear, activeCamera.zFar);
}

void RenderSystem::addSubsystem(std::shared_ptr<IRenderSubsystem> system) {
  m_renderSubsystems.emplace_back(system);
}

void RenderSystem::removeSubsystem(std::shared_ptr<IRenderSubsystem> system) {
  // todo
}

void RenderSystem::render() {
  // 1. Select buffer to render into
  auto view = m_registry.view<const Renderable>();

  for (auto entity : view) {
    const auto &renderable = view.get<const Renderable>(entity);
    const auto &name = m_registry.get<Name>(entity);

    // activate shader program
    glUseProgram(renderable.program);

    // prepare the materials
    m_materialSystem->onRender(entity);

    // TODO: disable in release
    if (renderable.isWireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // bind the vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, renderable.vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.index_buffer);

    for (unsigned int i = 0U; i < renderable.vertexAttribCount; ++i) {
      const VertexAttribArray &attrib = renderable.vertexAttribArray[i];
      glEnableVertexAttribArray(i);
      glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0,
                            BUFFER_OFFSET(attrib.buffer_offset));
    }

    //    glBindVertexArray(renderable.vertex_array_object);

    // assign textures
    for (auto &subsystem : m_renderSubsystems) {
      subsystem->onRender(m_registry, entity);
    }

    // TODO: check if shader is dirty
    //  reason: if we push properties every frame (Except for MVP), we might
    //  unnecessary spend time doing that while we can immediately just render.
    pushShaderProperties(renderable);
    pushModelViewProjection(renderable);

    // TODO this one needs to change per glUseProgram, which is the case right
    //  now. In future we might optimize changing of glUseProgram in that
    //  case, this function should be called once per set of glUseProgram.
    if (renderable.isLightingSupported) {
      pushLightingData(renderable, m_uLightData);
    }

    if (renderable.primitiveType == GL_TRIANGLES) {
      glDrawElements(renderable.primitiveType, renderable.drawElementCount,
                     GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(renderable.primitiveType, 0, renderable.drawElementCount);
    }

    // handle debug
    checkError(entity);
  }
}

void RenderSystem::checkError(entt::entity e) {
  // get error message
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    auto name = m_registry.has<Name>(e) ? m_registry.get<Name>(e).name
                                        : std::string{"Unnamed"};
    std::cerr << " Render error occurred for " << name << ": " << err
              << std::endl;
  }
}
