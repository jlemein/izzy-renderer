//
// Created by jlemein on 07-11-20.
//
#include <ecs_rendersystem.h>

#include <cstring>
#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <shp_curve.h>
#include <shp_mesh.h>
using namespace artifax;
using namespace artifax::ecs;

namespace {
std::vector<char> readFile(const std::string &filename) {
  // TODO: check for empty file name and give clear error message

  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "Failed to open file '" << filename << "'" << std::endl;
    throw std::runtime_error("failed to open file!");
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

constexpr void *BUFFER_OFFSET(unsigned int offset) {
  uint8_t *pAddress = 0;
  return pAddress + offset;
}

void pushShaderProperties(const Renderable &r) {
  for (const auto &[name, uniformBlock] : r.userProperties) {

    // TODO problem here: multiple shaders with same parameters (but they should
    // be different).
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockBinding,
                     uniformBlock.bufferId);

    // is this needed?
    glUniformBlockBinding(r.program, uniformBlock.blockIndex,
                          uniformBlock.blockBinding);

    void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    //    auto colors = reinterpret_cast<ColorBlock
    //    *>(uniformBlock.pData->data); std::cout << r.name << ": pushing " <<
    //    colors->color.r << " - "
    //              << colors->color.g << " - " << colors->color.b << std::endl;
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
    std::cerr << "Invalid ubo block index for renderable " << renderable.name
              << std::endl;
    exit(1);
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
  std::cout << "Renderable " << renderable.name
            << " | block: " << renderable.uboBlockIndex
            << ", binding: " << renderable.uboBlockBinding << std::endl;

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex,
                   renderable.uboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr,
               GL_DYNAMIC_DRAW);
}

void initLightingUbo(Renderable &renderable) {
  renderable.uboLightingIndex =
      glGetUniformBlockIndex(renderable.program, UniformLighting::PARAM_NAME);
  if (renderable.uboLightingIndex == GL_INVALID_INDEX) {
    std::cout << "Cannot find ubo block index with name "
              << UniformLighting::PARAM_NAME << " for " << renderable.name
              << std::endl;
    //    exit(1);
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
  std::cout << "Renderable " << renderable.name
            << " | block: " << renderable.uboLightingIndex
            << ", binding: " << renderable.uboLightingBinding << std::endl;

  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboLightingIndex,
                   renderable.uboLightingId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformLighting), nullptr,
               GL_DYNAMIC_DRAW);
}

void initShaderProperties(Renderable &renderable,
                          const Shader::UniformProperties &properties) {
  glUseProgram(renderable.program);

  // allocates the MVP matrices on the GPU.
  initMVPUniformBlock(renderable);
  initLightingUbo(renderable);

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

    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

    //    std::cout << " --- " << name << " has uboBlockIndex: " << blockIndex
    //    << std::endl;
    if (blockIndex == GL_INVALID_INDEX) {
      std::cerr << "Cannot find ubo block with name '" << name << "' in shader";
      exit(1);
    }
    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

    ColorBlock c;
    float rgb[4] = {1.0F, 1.0F, 1.0F, 1.0F};
    //    glBufferData(GL_UNIFORM_BUFFER, sizeof(ColorBlock), rgb,
    //    GL_DYNAMIC_DRAW);

    void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, rgb, sizeof(ColorBlock));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    auto p = reinterpret_cast<ColorBlock *>(blockData.data);
    std::cout << "   --- Init " << name << " with " << p->color.r << " "
              << p->color.g << " " << p->color.b << std::endl;

    // store block handle in renderable
    renderable.userProperties[name] = Renderable_UniformBlockInfo{
        uboHandle, blockIndex, blockBinding, &blockData};
  }
}

void checkError(std::string message) {
  // get error message
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << message << " Error occurred: " << err << std::endl;
  }
}

} // namespace

RenderSystem::RenderSystem(entt::registry &registry)
    : m_registry{registry}, m_debugSystem(registry) {}

void initCurveBuffers(Renderable &renderable, const shp::Curve &curve) {
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

void initMeshBuffers(Renderable &renderable, const shp::Mesh &mesh) {
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
GLuint compileShader(const Shader &shader, const Renderable &renderable) {
  auto vertexShaderBuffer = readFile(shader.vertexShaderFile);
  auto fragmentShaderBuffer = readFile(shader.fragmentShaderFile);

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

  if (!fsCompiled)
    getShaderLog(fragment_shader);

  if (!vsCompiled)
    getShaderLog(vertex_shader);

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
    std::cout << "Linking failed" << std::endl;
  }

  return program;
}

void RenderSystem::init() {
  glShadeModel(GL_SMOOTH);

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
  auto curveView = m_registry.view<shp::Curve, Renderable>();
  for (auto entity : curveView) {
    auto &curve = m_registry.get<shp::Curve>(entity);
    auto &renderable = m_registry.get<Renderable>(entity);
    auto &shader = m_registry.get<Shader>(entity);

    initCurveBuffers(renderable, curve);
    renderable.program = compileShader(shader, renderable);
    initShaderProperties(renderable, shader.properties);
  }

  // handling meshes
  auto view = m_registry.view<shp::Mesh, Shader, Renderable>();
  for (auto entity : view) {
    auto &mesh = m_registry.get<shp::Mesh>(entity);
    auto &renderable = m_registry.get<Renderable>(entity);
    auto &shader = m_registry.get<Shader>(entity);

    initMeshBuffers(renderable, mesh);
    renderable.program = compileShader(shader, renderable);
    initShaderProperties(renderable, shader.properties);
  }
}

void RenderSystem::update(float /*time*/, float /*dt*/) {
  // synchronizes the transformation for the entity into the renderable
  // component.
  synchMvpMatrices();
  updateLightProperties();
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

  if (view.size() > 4) {
    throw std::runtime_error(
        "Too many lights in the scene. Max supported is 4");
  }

  unsigned int i = 0U;
  for (auto e : view) {
    const auto &light = view.get<Light>(e);
    const auto &transform = view.get<Transform>(e);

    m_uLightData.colors[i] = glm::vec4(light.color, 1.0F);
    m_uLightData.intensities[i] = glm::vec4(light.intensity, 1.0F);
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

void RenderSystem::updateCamera(Renderable &renderable) {
  auto view = m_registry.view<ecs::Camera, ecs::Transform>();
  if (view.size() <= 0) {
    std::cout << "No active camera in scene" << std::endl;
    return;
  }

  // from camera take the position and perspective transformation
  auto &activeCamera = m_registry.get<ecs::Camera>(*view.begin());
  auto &transform = m_registry.get<ecs::Transform>(*view.begin());

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
  auto view = m_registry.view<const Renderable>();

  for (auto entity : view) {
    const auto &renderable = view.get<const Renderable>(entity);

    // activate shader program
    glUseProgram(renderable.program);

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

    for (auto &subsystem : m_renderSubsystems) {
      subsystem->onRender(m_registry, entity);
    }

    // TODO: check if shader is dirty
    pushShaderProperties(renderable);

    pushModelViewProjection(renderable);

    // TODO this one needs to change per glUseProgram, which is the case right
    //  now. In future we might optimize changing of glUseProgram in that case,
    //  this function should be called once per set of glUseProgram.
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

    checkError("after render " + renderable.name);
  }
}
