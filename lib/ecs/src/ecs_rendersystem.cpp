//
// Created by jlemein on 07-11-20.
//
#include <ecs_rendersystem.h>

#include <ecs_camera.h>
#include <ecs_transform.h>
#include <shp_curve.h>
#include <shp_mesh.h>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
using namespace artifax;
using namespace artifax::ecs;

namespace
{
std::vector<char>
readFile(const std::string &filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "Failed to open file " << filename;
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

void
getShaderLog(GLint shader)
{
  std::cout << "Failed to compile shader" << std::endl;
  GLint reqBufferSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &reqBufferSize);

  std::vector<char> buffer;
  buffer.resize(reqBufferSize);
  glGetShaderInfoLog(shader, reqBufferSize, nullptr, buffer.data());

  std::string log(buffer.begin(), buffer.end());
  std::cout << log << std::endl;
}

constexpr void *
BUFFER_OFFSET(unsigned int offset)
{
  uint8_t *pAddress = 0;
  return pAddress + offset;
}

void
pushModelViewProjection(const Renderable &renderable)
{
  //  std::cout << renderable.name << ": binding mvp.index = " << renderable.uboBlockIndex
  //  << ", " << renderable.ubo_handle << std::endl;

  glBindBuffer(GL_UNIFORM_BUFFER, renderable.ubo_handle);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex, renderable.ubo_handle);
  //  glUniformBlockBinding(renderable.program, renderable.uboBlockIndex, 0);
  void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
  std::memcpy(buff_ptr, &renderable.uniformBlock, sizeof(UniformBlock));
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void
initMVPUniformBlock(Renderable &renderable)
{
  glUseProgram(renderable.program);
  renderable.uboBlockIndex = glGetUniformBlockIndex(renderable.program,
                                                    "UniformBufferBlock");
  if (renderable.uboBlockIndex == GL_INVALID_INDEX) {
    std::cerr << "Invalid ubo block index for renderable " << renderable.name
              << std::endl;
    exit(1);
  }

  glGenBuffers(1, &renderable.ubo_handle);
  glBindBufferBase(GL_UNIFORM_BUFFER, renderable.uboBlockIndex, renderable.ubo_handle);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), &renderable.uniformBlock,
               GL_DYNAMIC_DRAW);
}

void
initShaderProperties(Renderable &renderable, const Shader::UniformProperties &properties)
{
  std::cout << "For renderable: " << renderable.name << std::endl;

  glUseProgram(renderable.program);
  initMVPUniformBlock(renderable);

  std::cout << " -- number of uniform blocks: " << properties.size() << std::endl;

  for (const auto &[name, blockData] : properties) {
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    GLint blockIndex = glGetUniformBlockIndex(renderable.program, name.c_str());
    GLint blockBinding;
    glGetActiveUniformBlockiv(renderable.program, blockIndex, GL_UNIFORM_BLOCK_BINDING,
                              &blockBinding);

    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

    //    std::cout << " --- " << name << " has uboBlockIndex: " << blockIndex <<
    //    std::endl;
    if (blockIndex == GL_INVALID_INDEX) {
      std::cerr << "Cannot find ubo block with name '" << name << "' in shader";
      exit(1);
    }
    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

    // Each active uniform block in GLSL has a corresponding
    // active uniform block in the linked program.

    std::cout << " -- " << name << " blockIndex: " << blockIndex << std::endl;

    GLint bi = glGetUniformBlockIndex(renderable.program, "UniformBufferBlock");
    std::cout << " -- "
              << "UniformBufferBlock blockIndex: " << bi << std::endl;

    ColorBlock c;
    float rgb[4] = {1.0F, 1.0F, 1.0F, 1.0F};
    //    glBufferData(GL_UNIFORM_BUFFER, sizeof(ColorBlock), rgb, GL_DYNAMIC_DRAW);

    void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, rgb, sizeof(ColorBlock));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    auto p = reinterpret_cast<ColorBlock *>(blockData.data);
    std::cout << "   --- Init " << name << " with " << p->color.r << " " << p->color.g
              << " " << p->color.b << std::endl;

    // store block handle in renderable
    renderable.userProperties[name] = Renderable_UniformBlockInfo{
      uboHandle, blockIndex, blockBinding, &blockData};
  }
}

void
pushUniforms(const Renderable &r)
{
  for (const auto &[name, uniformBlock] : r.userProperties) {
    //    std::cout << r.name << ": binding uniformBlock.index = " <<
    //    uniformBlock.blockIndex
    //              << ", " << uniformBlock.bufferId << " (size: " <<
    //              uniformBlock.pData->size
    //              << ")" << std::endl;

    // TODO problem here
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.blockIndex, uniformBlock.bufferId);
    //    glUniformBlockBinding(r.program, uniformBlock.blockIndex,
    //    uniformBlock.blockBinding);

    void *buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, uniformBlock.pData->data, uniformBlock.pData->size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    auto colors = reinterpret_cast<ColorBlock *>(uniformBlock.pData->data);
    //    std::cout << r.name << ": pushing " << colors->color.r << " - " <<
    //    colors->color.g
    //              << " - " << colors->color.b << std::endl;
  }
}

void
checkError(std::string message)
{
  // get error message
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << message << " Error occurred: " << err << std::endl;
  }
}

}  // namespace

RenderSystem::RenderSystem(entt::registry &registry)
    : m_registry{registry}, m_debugSystem(registry)
{
}

void
RenderSystem::updateCamera(Renderable &renderable, float time, float dt)
{
  auto view = m_registry.view<ecs::Camera, ecs::Transform>();
  if (view.size() <= 0) {
    std::cout << "No active camera in scene" << std::endl;
    return;
  }

  // from camera take the position and perspective transformation
  auto &activeCamera = m_registry.get<ecs::Camera>(*view.begin());
  auto &transform = m_registry.get<ecs::Transform>(*view.begin());

  renderable.uniformBlock.view = glm::inverse(transform.worldTransform);
  renderable.uniformBlock.proj = glm::perspective(activeCamera.fovx, activeCamera.aspect,
                                                  activeCamera.zNear, activeCamera.zFar);
}

void
initCurveBuffers(Renderable &renderable, const shp::Curve &curve)
{
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

void
initMeshBuffers(Renderable &renderable, const shp::Mesh &mesh)
{
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize;

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
GLuint
compileShader(const Shader &shader, const Renderable &renderable)
{
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

void
RenderSystem::init()
{
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

#ifndef NDEBUG
  m_debugSystem.init();
#endif //NDEBUG
}

void
RenderSystem::update(float time, float dt)
{
  auto view = m_registry.view<Renderable>();
  for (auto entity : view) {
    auto &renderable = m_registry.get<Renderable>(entity);
    auto transform = m_registry.try_get<ecs::Transform>(entity);
    if (transform != nullptr) {
      renderable.uniformBlock.model = transform->worldTransform;
    } else {
      renderable.uniformBlock.model = glm::mat4(1.0F);
    }

    // update uniform parameters
    updateCamera(renderable, time, dt);
  }
}

void
RenderSystem::addSubsystem(std::shared_ptr<IRenderSubsystem> system)
{
  m_renderSubsystems.emplace_back(system);
}

void
RenderSystem::removeSubsystem(std::shared_ptr<IRenderSubsystem> system)
{
  // todo
}

void
RenderSystem::render()
{
  auto view = m_registry.view<const Renderable>();

  for (auto entity : view) {
    const auto &renderable = view.get<const Renderable>(entity);

    // activate shader program
    glUseProgram(renderable.program);

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
    pushUniforms(renderable);

    pushModelViewProjection(renderable);

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
