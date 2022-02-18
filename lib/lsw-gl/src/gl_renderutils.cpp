//
// Created by jeffrey on 14-02-22.
//
#include <geo_curve.h>
#include <geo_mesh.h>
#include <gl_renderutils.h>
#include "gl_shadersystem.h"
#include <gl_renderable.h>
using namespace izz;
using namespace izz::gl;

namespace {
void loadUnscopedUniformParameters(const lsw::geo::Material& material, RenderState& rs);
int getUniformLocation(GLint program, const char* name, const std::string& materialName);
}  // namespace
constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void RenderUtils::FillBufferedMeshData(const lsw::geo::Curve& curve, BufferedMeshData& md) {
  glGenBuffers(1, &md.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, curve.vertices.size() * sizeof(float), curve.vertices.data(), GL_STATIC_DRAW);

  md.index_buffer = 0U;
  //  renderable.vertexAttribCount = 1;
  //  renderable.vertexAttribArray[0].size = 3U;
  //  renderable.vertexAttribArray[0].buffer_offset = 0U;
  md.drawElementCount = curve.vertices.size() / 3;
  md.primitiveType = GL_LINES;

  glGenVertexArrays(1, &md.vertex_array_object);
  glBindVertexArray(md.vertex_array_object);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  // disable the buffers to prevent accidental manipulation
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderUtils::FillBufferedMeshData(const lsw::geo::Mesh& mesh, BufferedMeshData& md) {
  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
  GLuint normalSize = mesh.normals.size() * sizeof(float);
  GLuint uvSize = mesh.uvs.size() * sizeof(float);
  GLuint tangentSize = mesh.tangents.size() * sizeof(float);
  GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
  GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;

  glGenBuffers(1, &md.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
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

  glGenBuffers(1, &md.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

  md.vertexAttribCount = 5;
  // position data
  md.vertexAttribArray[0].size = 3U;
  md.vertexAttribArray[0].buffer_offset = 0U;
  // normal data
  md.vertexAttribArray[1].size = 3U;
  md.vertexAttribArray[1].buffer_offset = vertexSize;
  // uv data
  md.vertexAttribArray[2].size = 2U;
  md.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
  // tangent data
  md.vertexAttribArray[3].size = 3U;
  md.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
  // bitangent data
  md.vertexAttribArray[4].size = 3U;
  md.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;

  md.drawElementCount = mesh.indices.size();
  md.primitiveType = GL_TRIANGLES;

  glGenVertexArrays(1, &md.vertex_array_object);
  glBindVertexArray(md.vertex_array_object);
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

void RenderUtils::ActivateTextures(const RenderState& rs) {
  for (int t = 0; t < rs.textureBuffers.size(); ++t) {
    const auto& texture = rs.textureBuffers[t];

    glActiveTexture(GL_TEXTURE0 + t);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glUniform1i(texture.uniformLocation, t);
  }
}

void RenderUtils::UseBufferedMeshData(const BufferedMeshData& md) {
  // bind the vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
  //    glBindVertexArray(renderable.vertex_array_object);

  for (unsigned int i = 0U; i < md.vertexAttribCount; ++i) {
    const VertexAttribArray& attrib = md.vertexAttribArray[i];
    // todo: use VAOs
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
  }
}

void RenderUtils::ActivateUniformProperties(const RenderState& rs) {
  for (const auto& uniform : rs.uniformBlocks) {
    glBindBuffer(GL_UNIFORM_BUFFER, uniform.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniform.blockBind, uniform.bufferId);

    // is this needed?
    glUniformBlockBinding(rs.program, uniform.blockIndex, uniform.blockBind);

    void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    std::memcpy(buff_ptr, uniform.pData, uniform.size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }

  ActivateUnscopedUniforms(rs);
}

void RenderUtils::ActivateUnscopedUniforms(const RenderState& rs) {
  for (unsigned int i = 0; i < rs.unscopedUniforms.numProperties; ++i) {
    const auto& uniform = rs.unscopedUniforms.pProperties[i];

    switch (uniform.type) {
      case UType::BOOL:
      case UType::INT:
        glUniform1i(uniform.location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + uniform.offset));
        break;

      case UType::FLOAT_ARRAY:
      case UType::FLOAT2:
      case UType::FLOAT3:
      case UType::FLOAT4:
        glUniform1fv(uniform.location, uniform.length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + uniform.offset));
        break;

      case UType::INT_ARRAY:
      case UType::INT2:
      case UType::INT3:
      case UType::INT4:
        glUniform1iv(uniform.location, uniform.length, reinterpret_cast<GLint*>(rs.unscopedUniforms.pData + uniform.offset));
        break;
    }
  }
}

namespace {
void LoadUniformParameters(const lsw::geo::Material& m, RenderState& rs) {
  for (const auto& [name, blockData] : m.uniformBlocks) {
    GLuint uboHandle;
    glGenBuffers(1, &uboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

    GLint blockIndex = glGetUniformBlockIndex(rs.program, name.c_str());
    GLint blockBinding;
    glGetActiveUniformBlockiv(rs.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);

    // is this needed?
    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);

    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);

    if (blockIndex == GL_INVALID_INDEX) {
      auto a = glGetUniformLocation(rs.program, name.c_str());
      throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, m.name));
    }
    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);

    // store block handle in renderable
    UniformBufferMapping mapping;
//    Renderable_UniformBlockInfo block{};
    mapping.bufferId = uboHandle;
    mapping.blockIndex = blockIndex;
    mapping.blockBind = blockBinding;
    mapping.pData = blockData.data;
    mapping.size = blockData.size;
    rs.uniformBlocks.push_back(mapping);
  }

  loadUnscopedUniformParameters(m, rs);
}

void loadUnscopedUniformParameters(const lsw::geo::Material& material, RenderState& rs) {
  // first memory allocation. For this we need to know the number of properties and length of data properties.
  int numProperties = material.unscopedUniforms.booleanValues.size() + material.unscopedUniforms.intValues.size() +
                      material.unscopedUniforms.floatValues.size() + material.unscopedUniforms.floatArrayValues.size();
  uint64_t sizeBytes = 0U;
  sizeBytes += material.unscopedUniforms.booleanValues.size() * sizeof(GLint);
  sizeBytes += material.unscopedUniforms.intValues.size() * sizeof(GLint);
  sizeBytes += material.unscopedUniforms.floatValues.size() * sizeof(GLfloat);
  for (const auto& array : material.unscopedUniforms.floatArrayValues) {
    sizeBytes += array.second.size() * sizeof(GLfloat);
  }

  rs.unscopedUniforms = UnscopedUniforms::Allocate(numProperties, sizeBytes);

  auto pData = rs.unscopedUniforms.pData;
  auto pUniform = rs.unscopedUniforms.pProperties;
  unsigned int offset = 0U;

  for (auto [name, value] : material.unscopedUniforms.booleanValues) {
    *reinterpret_cast<int*>(pData) = value;
    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
    pUniform->type = UType::BOOL;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(GLint);
    pData += sizeof(GLint);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.intValues) {
    *reinterpret_cast<int*>(pData) = value;
    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
    pUniform->type = UType::INT;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(GLint);
    pData += sizeof(GLint);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.floatValues) {
    *reinterpret_cast<float*>(pData) = value;
    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
    pUniform->type = UType::FLOAT;
    pUniform->offset = offset;
    pUniform->length = 1;

    glUniform1f(pUniform->location, *reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

    offset += sizeof(float);
    pData += sizeof(float);
    pUniform++;
  }

  for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
    memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
    pUniform->type = UType::FLOAT_ARRAY;
    pUniform->offset = offset;
    pUniform->length = value.size();

    glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
    spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));

    offset += sizeof(float) * value.size();
    pData += sizeof(float) * value.size();
    pUniform++;
  }
}

int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
  int location = glGetUniformLocation(program, name);
  if (location == -1) {
    throw std::runtime_error(fmt::format("unknown uniform parameter '{}' (material: '{}').", name, materialName));
  }
  return location;
}
}  // namespace

void RenderUtils::LoadMaterial(const lsw::geo::Material& material, RenderState& rs) {
  ShaderSystem shaderCompiler;
  try {
    if (material.isBinaryShader) {
      rs.program = shaderCompiler.compileSpirvShader(material.vertexShader, material.fragmentShader);
    } else {
      rs.program = shaderCompiler.compileShader(material.vertexShader, material.fragmentShader);
    }

    spdlog::debug("#{} Shader program compiled successfully (vs: {} fs: {})", rs.program, material.vertexShader, material.fragmentShader);

    for (const auto& [name, uniform] : material.uniformBlocks) {
      GLuint uboHandle;
      glGenBuffers(1, &uboHandle);
      glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

      GLint blockIndex = glGetUniformBlockIndex(rs.program, name.c_str());
      GLint blockBinding;
      glGetActiveUniformBlockiv(rs.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);

      // is this needed?
      //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);

      glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);

      if (blockIndex == GL_INVALID_INDEX) {
        auto a = glGetUniformLocation(rs.program, name.c_str());
        throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, material.name));
      }
      glBufferData(GL_UNIFORM_BUFFER, uniform.size, NULL, GL_DYNAMIC_DRAW);

      // store block handle in renderable
      UniformBufferMapping mapping;
      //    mapping.name = name;
      mapping.bufferId = uboHandle;
      mapping.blockIndex = blockIndex;
      mapping.blockBind = blockBinding;
      mapping.size = uniform.size;
      mapping.pData = uniform.data;
      rs.uniformBlocks.push_back(mapping);
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

    rs.unscopedUniforms = UnscopedUniforms::Allocate(numProperties, sizeBytes);

    auto pData = rs.unscopedUniforms.pData;
    auto pUniform = rs.unscopedUniforms.pProperties;
    unsigned int offset = 0U;

    for (auto [name, value] : material.unscopedUniforms.booleanValues) {
      *reinterpret_cast<int*>(pData) = value;
      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
      pUniform->type = UType::BOOL;
      pUniform->offset = offset;
      pUniform->length = 1;

      glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

      offset += sizeof(GLint);
      pData += sizeof(GLint);
      pUniform++;
    }

    for (auto [name, value] : material.unscopedUniforms.intValues) {
      *reinterpret_cast<int*>(pData) = value;
      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
      pUniform->type = UType::INT;
      pUniform->offset = offset;
      pUniform->length = 1;

      glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

      offset += sizeof(GLint);
      pData += sizeof(GLint);
      pUniform++;
    }

    for (auto [name, value] : material.unscopedUniforms.floatValues) {
      *reinterpret_cast<float*>(pData) = value;
      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
      pUniform->type = UType::FLOAT;
      pUniform->offset = offset;
      pUniform->length = 1;

      glUniform1f(pUniform->location, *reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);

      offset += sizeof(float);
      pData += sizeof(float);
      pUniform++;
    }

    for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
      memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
      pUniform->type = UType::FLOAT_ARRAY;
      pUniform->offset = offset;
      pUniform->length = value.size();

      glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
      spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));

      offset += sizeof(float) * value.size();
      pData += sizeof(float) * value.size();
      pUniform++;
    }
  } catch (std::exception& e) {
    //    auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
    throw std::runtime_error(fmt::format("Failed loading material ", e.what()));
  }
}

UniformBufferMapping RenderUtils::GetUniformBufferLocation(const RenderState& rs, std::string uboName) {
  glUseProgram(rs.program);  // TODO: remove line

  int uboBlockIndex = glGetUniformBlockIndex(rs.program, uboName.c_str());
  if (uboBlockIndex == GL_INVALID_INDEX) {
    //    throw std::runtime_error(fmt::format("Shader program does not contain a uniform block with name 'UniformBufferBlock' in {}",
    //                                         renderable.material ? renderable.material->vertexShader : "<no material assigned>"));
    throw std::runtime_error(fmt::format("Could not find UBO with name {}", uboName));
  }

  //  renderable.isMvpSupported = true;
  GLuint bufferId;
  glGenBuffers(1, &bufferId);
  glBindBuffer(GL_UNIFORM_BUFFER, bufferId);

  int uboBlockBinding;
  glGetActiveUniformBlockiv(rs.program, uboBlockIndex, GL_UNIFORM_BLOCK_BINDING, &uboBlockBinding);

  glBindBufferBase(GL_UNIFORM_BUFFER, uboBlockIndex, bufferId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), nullptr, GL_DYNAMIC_DRAW);

  UniformBufferMapping mapping;
  mapping.blockIndex = uboBlockIndex;
  mapping.blockBind = uboBlockBinding;
  mapping.size = sizeof(UniformBlock);
  return mapping;
}