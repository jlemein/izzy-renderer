//
// Created by jeffrey on 14-02-22.
//
#include <geo_curve.h>
#include <geo_mesh.h>
#include <gl_renderable.h>
#include <gl_renderutils.h>
#include "izzgl_shadersystem.h"
using namespace izz;
using namespace izz::gl;

namespace {
void loadUnscopedUniformParameters(const Material& material, RenderState& rs);
int getUniformLocation(GLint program, const char* name, const std::string& materialName);
}  // namespace
constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void RenderUtils::FillBufferedMeshData(const izz::geo::Curve& curve, MeshBuffer& md) {
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
//
//void RenderUtils::FillBufferedMeshData(const lsw::geo::Mesh& mesh, MeshBuffer& md) {
//  GLuint vertexSize = mesh.vertices.size() * sizeof(float);
//  GLuint normalSize = mesh.normals.size() * sizeof(float);
//  GLuint uvSize = mesh.uvs.size() * sizeof(float);
//  GLuint tangentSize = mesh.tangents.size() * sizeof(float);
//  GLuint bitangentSize = mesh.bitangents.size() * sizeof(float);
//  GLuint bufferSize = vertexSize + normalSize + uvSize + tangentSize + bitangentSize;
//  int vertexAttribCount = 0;
//
//  glGenBuffers(1, &md.vertex_buffer);
//  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
//  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);  // allocate buffer data only
//
//  if (vertexSize > 0) {
//    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, mesh.vertices.data());  // fill partial data - vertices
//    ++vertexAttribCount;
//  }
//  if (normalSize > 0) {
//    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalSize, mesh.normals.data());  // fill partial data - normals
//    ++vertexAttribCount;
//  }
//  if (uvSize > 0) {
//    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize, uvSize, mesh.uvs.data());  // fill partial data - normals
//    ++vertexAttribCount;
//  }
//  if (tangentSize > 0) {
//    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize, tangentSize, mesh.tangents.data());  // fill partial data - normals
//    ++vertexAttribCount;
//  }
//  if (bitangentSize > 0) {
//    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalSize + uvSize + tangentSize, bitangentSize, mesh.bitangents.data());  // fill partial data - normals
//    ++vertexAttribCount;
//  }
//
//  glGenBuffers(1, &md.index_buffer);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
//
//  md.vertexAttribCount = vertexAttribCount;
//  // position data
//  md.vertexAttribArray[0].size = 3U;
//  md.vertexAttribArray[0].buffer_offset = 0U;
//  // normal data
//  md.vertexAttribArray[1].size = 3U;
//  md.vertexAttribArray[1].buffer_offset = vertexSize;
//  // uv data
//  md.vertexAttribArray[2].size = 2U;
//  md.vertexAttribArray[2].buffer_offset = vertexSize + normalSize;
//  // tangent data
//  md.vertexAttribArray[3].size = 3U;
//  md.vertexAttribArray[3].buffer_offset = vertexSize + normalSize + uvSize;
//  // bitangent data
//  md.vertexAttribArray[4].size = 3U;
//  md.vertexAttribArray[4].buffer_offset = vertexSize + normalSize + uvSize + tangentSize;
//
//  md.drawElementCount = mesh.indices.size();
//  md.primitiveType = GL_TRIANGLES;
//
//  glGenVertexArrays(1, &md.vertex_array_object);
//  glBindVertexArray(md.vertex_array_object);
//  glEnableVertexAttribArray(0);
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
//  glEnableVertexAttribArray(1);
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize));
//  glEnableVertexAttribArray(2);
//  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize));
//  glEnableVertexAttribArray(3);
//  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize));
//  glEnableVertexAttribArray(4);
//  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexSize + normalSize + uvSize + tangentSize));
//  glBindVertexArray(0);
//}

//void RenderUtils::ActivateTextures(const RenderState& rs) {
//  for (int t = 0; t < rs.textureBuffers.size(); ++t) {
//    const auto& texture = rs.textureBuffers[t];
//
//    std::cout << "Activating texture " << t << std::endl;
//
//    glActiveTexture(GL_TEXTURE0 + t);
//    glBindTexture(GL_TEXTURE_2D, texture.textureId);
//    glUniform1i(texture.uniformLocation, t);
//  }
//}
//
//void RenderUtils::UseBufferedMeshData(const MeshBuffer& md) {
//  // bind the vertex buffers
//  glBindBuffer(GL_ARRAY_BUFFER, md.vertex_buffer);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.index_buffer);
//  //    glBindVertexArray(renderable.vertex_array_object);
//
//  for (unsigned int i = 0U; i < md.vertexAttribCount; ++i) {
//    const VertexAttribArray& attrib = md.vertexAttribArray[i];
//    // todo: use VAOs
//    glEnableVertexAttribArray(i);
//    glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrib.buffer_offset));
//  }
//}

//void RenderUtils::PushUniformProperties(const RenderState& rs) {
//  for (const auto& uniform : rs.uniformBlocks) {
//    glBindBuffer(GL_UNIFORM_BUFFER, uniform.bufferId);
//    glBindBufferBase(GL_UNIFORM_BUFFER, uniform.blockBind, uniform.bufferId);
//
//    // is this needed?
//    glUniformBlockBinding(rs.program, uniform.blockIndex, uniform.blockBind);
//
//    void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
//    std::memcpy(buff_ptr, uniform.data, uniform.size);
//    glUnmapBuffer(GL_UNIFORM_BUFFER);
//  }
//
//  ActivateUnscopedUniforms(rs);
//}

//void RenderUtils::ActivateUnscopedUniforms(const RenderState& rs) {
//  for (unsigned int i = 0; i < rs.unscopedUniforms.numProperties; ++i) {
//    const auto& uniform = rs.unscopedUniforms.pProperties[i];
//
//    switch (uniform.type) {
//      case UType::BOOL:
//      case UType::INT:
//        glUniform1i(uniform.location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + uniform.offset));
//        break;
//
//      case UType::FLOAT_ARRAY:
//      case UType::FLOAT2:
//      case UType::FLOAT3:
//      case UType::FLOAT4:
//        glUniform1fv(uniform.location, uniform.length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + uniform.offset));
//        break;
//
//      case UType::INT_ARRAY:
//      case UType::INT2:
//      case UType::INT3:
//      case UType::INT4:
//        glUniform1iv(uniform.location, uniform.length, reinterpret_cast<GLint*>(rs.unscopedUniforms.pData + uniform.offset));
//        break;
//    }
//  }
//}

//namespace {
//void LoadUniformParameters(const Material& m, RenderState& rs) {
//  for (const auto& [name, blockData] : m.uniformBlocks) {
//    GLuint uboHandle;
//    glGenBuffers(1, &uboHandle);
//    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//
//    GLint blockIndex = glGetUniformBlockIndex(rs.program, name.c_str());
//    GLint blockBinding;
//    glGetActiveUniformBlockiv(rs.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
//
//    // is this needed?
//    //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);
//
//    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);
//
//    if (blockIndex == GL_INVALID_INDEX) {
//      auto a = glGetUniformLocation(rs.program, name.c_str());
//      throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, m.name));
//    }
//    glBufferData(GL_UNIFORM_BUFFER, blockData.size, NULL, GL_DYNAMIC_DRAW);
//
//    // store block handle in renderable
//    UniformBufferMapping mapping;
//    //    Renderable_UniformBlockInfo block{};
//    mapping.bufferId = uboHandle;
//    mapping.blockIndex = blockIndex;
//    mapping.blockBind = blockBinding;
//    mapping.pData = blockData.data;
//    mapping.size = blockData.size;
//    rs.uniformBlocks.push_back(mapping);
//  }
//
//  loadUnscopedUniformParameters(m, rs);
//}
//
//void loadUnscopedUniformParameters(const Material& material, RenderState& rs) {
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
//  rs.unscopedUniforms = UnscopedUniforms::Allocate(numProperties, sizeBytes);
//
//  auto pData = rs.unscopedUniforms.pData;
//  auto pUniform = rs.unscopedUniforms.pProperties;
//  unsigned int offset = 0U;
//
//  for (auto [name, value] : material.unscopedUniforms.booleanValues) {
//    *reinterpret_cast<int*>(pData) = value;
//    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//    pUniform->type = UType::BOOL;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(GLint);
//    pData += sizeof(GLint);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.intValues) {
//    *reinterpret_cast<int*>(pData) = value;
//    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//    pUniform->type = UType::INT;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(GLint);
//    pData += sizeof(GLint);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.floatValues) {
//    *reinterpret_cast<float*>(pData) = value;
//    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//    pUniform->type = UType::FLOAT;
//    pUniform->offset = offset;
//    pUniform->length = 1;
//
//    glUniform1f(pUniform->location, *reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//    offset += sizeof(float);
//    pData += sizeof(float);
//    pUniform++;
//  }
//
//  for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
//    memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
//    pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//    pUniform->type = UType::FLOAT_ARRAY;
//    pUniform->offset = offset;
//    pUniform->length = value.size();
//
//    glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
//    spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));
//
//    offset += sizeof(float) * value.size();
//    pData += sizeof(float) * value.size();
//    pUniform++;
//  }
//}

//void RenderUtils::LoadMaterial(const Material& material, RenderState& rs) {
//  ShaderSystem shaderCompiler;
//  try {
//    spdlog::debug("Loading material with name: {}", material.name);
//
//    if (material.isBinaryShader) {
//      rs.program = shaderCompiler.compileSpirvShader(material.vertexShader, material.fragmentShader);
//    } else {
//      rs.program = shaderCompiler.compileShader(material.vertexShader, material.fragmentShader);
//    }
//    spdlog::debug("\tProgram id: {}\n\tShader program compiled successfully (vs: {} fs: {})", rs.program, material.vertexShader, material.fragmentShader);

//    // load textures - this means, create
//    for (const auto& [texname, buffer] : material.textures) {
////    for (const auto& [texname, filepath] : material.texturePaths) {
//      spdlog::debug("Texture {}: {}", texname, buffer.textureBufferId);
//
//      // When filepath is empty then the texture need to be defined by the user in code (i.e. there is no default).
//      // Or it is a texture that is passed in from previous render pass.
//      tb.uniformLocation = glGetUniformLocation(rs.program, texname.c_str());
//
//      if (!filepath.empty()) {
//        // TODO: load texture from file.
//      }
//
//      if (tb.uniformLocation != GL_INVALID_INDEX) {
//        spdlog::debug("Material {}: added texture {}: '{}'", material.name, texname, filepath);
//        rs.textureBuffers.emplace_back(tb);
//      } else {
//        spdlog::warn("Material {} defines texture \"{}\", but it is not found in the shader.", material.name, texname);
//      }
//
//      spdlog::debug("Texture {}: {} -> {}", texname, tb.textureId, tb.uniformLocation);
//
//      //      TextureBuffer tb;
//      //      tb.
//      //      rs.textureBuffers
//    }

//    for (const auto& [name, uniform] : material.uniformBlocks) {
//      spdlog::debug("\tUBO with name: {}", name);
//      GLuint uboHandle;
//      glGenBuffers(1, &uboHandle);
//      glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
//
//      GLint blockIndex = glGetUniformBlockIndex(rs.program, name.c_str());
//      if (blockIndex == GL_INVALID_INDEX) {
//        auto a = glGetUniformLocation(rs.program, name.c_str());
//        throw std::runtime_error(fmt::format("Cannot find ubo block with name '{}' in shader {}", name, material.name));
//      }
//
//      GLint blockBinding;
//      glGetActiveUniformBlockiv(rs.program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
//
//      // is this needed?
//      //    glUniformBlockBinding(renderable.program, blockIndex, blockBinding);
//
//      glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, uboHandle);
//
//      glBufferData(GL_UNIFORM_BUFFER, uniform.size, NULL, GL_DYNAMIC_DRAW);
//
//      // store block handle in renderable
//      UniformBufferMapping mapping;
//      //    mapping.name = name;
//      mapping.bufferId = uboHandle;
//      mapping.blockIndex = blockIndex;
//      mapping.blockBind = blockBinding;
//      mapping.size = uniform.size;
//      mapping.data = uniform.data;
//      rs.uniformBlocks.push_back(mapping);
//    }
//
//    // INIT UNSCOPED UNIFORMS
//    //  first memory allocation. For this we need to know the number of properties and length of data properties.
//    int numProperties = material.unscopedUniforms.booleanValues.size() + material.unscopedUniforms.intValues.size() +
//                        material.unscopedUniforms.floatValues.size() + material.unscopedUniforms.floatArrayValues.size();
//    uint64_t sizeBytes = 0U;
//    sizeBytes += material.unscopedUniforms.booleanValues.size() * sizeof(GLint);
//    sizeBytes += material.unscopedUniforms.intValues.size() * sizeof(GLint);
//    sizeBytes += material.unscopedUniforms.floatValues.size() * sizeof(GLfloat);
//    for (const auto& array : material.unscopedUniforms.floatArrayValues) {
//      sizeBytes += array.second.size() * sizeof(GLfloat);
//    }
//
//    rs.unscopedUniforms = UnscopedUniforms::Allocate(numProperties, sizeBytes);
//
//    auto pData = rs.unscopedUniforms.pData;
//    auto pUniform = rs.unscopedUniforms.pProperties;
//    unsigned int offset = 0U;
//
//    for (auto [name, value] : material.unscopedUniforms.booleanValues) {
//      *reinterpret_cast<int*>(pData) = value;
//      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//      pUniform->type = UType::BOOL;
//      pUniform->offset = offset;
//      pUniform->length = 1;
//
//      glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
//      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//      offset += sizeof(GLint);
//      pData += sizeof(GLint);
//      pUniform++;
//    }
//
//    for (auto [name, value] : material.unscopedUniforms.intValues) {
//      *reinterpret_cast<int*>(pData) = value;
//      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//      pUniform->type = UType::INT;
//      pUniform->offset = offset;
//      pUniform->length = 1;
//
//      glUniform1i(pUniform->location, *reinterpret_cast<int*>(rs.unscopedUniforms.pData + pUniform->offset));
//      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//      offset += sizeof(GLint);
//      pData += sizeof(GLint);
//      pUniform++;
//    }
//
//    for (auto [name, value] : material.unscopedUniforms.floatValues) {
//      *reinterpret_cast<float*>(pData) = value;
//      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//      pUniform->type = UType::FLOAT;
//      pUniform->offset = offset;
//      pUniform->length = 1;
//
//      glUniform1f(pUniform->location, *reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
//      spdlog::info("Initialized: {}.{} = {}", material.name, name, value);
//
//      offset += sizeof(float);
//      pData += sizeof(float);
//      pUniform++;
//    }
//
//    for (auto [name, value] : material.unscopedUniforms.floatArrayValues) {
//      memcpy(reinterpret_cast<float*>(pData), value.data(), sizeof(float) * value.size());
//      pUniform->location = getUniformLocation(rs.program, name.c_str(), material.name);
//      pUniform->type = UType::FLOAT_ARRAY;
//      pUniform->offset = offset;
//      pUniform->length = value.size();
//
//      glUniform1fv(pUniform->location, pUniform->length, reinterpret_cast<float*>(rs.unscopedUniforms.pData + pUniform->offset));
//      spdlog::info("Initialized: {}.{} = [{}]", material.name, name, fmt::join(value, ", "));
//
//      offset += sizeof(float) * value.size();
//      pData += sizeof(float) * value.size();
//      pUniform++;
//    }
//  } catch (std::exception& e) {
//    //    auto name = m_registry.all_of<lsw::ecs::Name>(entity) ? m_registry.get<lsw::ecs::Name>(entity).name : "Unnamed";
//    throw std::runtime_error(fmt::format("Failed loading material: {}", e.what()));
//  }
//}

//UniformBufferMapping RenderUtils::GetUniformBufferLocation(const RenderState& rs, std::string uboName) {
//  glUseProgram(rs.program);  // TODO: remove line
//
//  int uboBlockIndex = glGetUniformBlockIndex(rs.program, uboName.c_str());
//  if (uboBlockIndex == GL_INVALID_INDEX) {
//    //    throw std::runtime_error(fmt::format("Shader program does not contain a uniform block with name 'UniformBufferBlock' in {}",
//    //                                         renderable.material ? renderable.material->vertexShader : "<no material assigned>"));
//    throw std::runtime_error(fmt::format("Could not find UBO with name {}", uboName));
//  }
//
//  //  renderable.isMvpSupported = true;
//  GLuint bufferId;
//  glGenBuffers(1, &bufferId);
//  glBindBuffer(GL_UNIFORM_BUFFER, bufferId);
//
//  int uboBlockBinding;
//  glGetActiveUniformBlockiv(rs.program, uboBlockIndex, GL_UNIFORM_BLOCK_BINDING, &uboBlockBinding);
//
//  glBindBufferBase(GL_UNIFORM_BUFFER, uboBlockIndex, bufferId);
//  glBufferData(GL_UNIFORM_BUFFER, sizeof(ModelViewProjection), nullptr, GL_DYNAMIC_DRAW);
//
//  UniformBufferMapping mapping;
//  mapping.blockIndex = uboBlockIndex;
//  mapping.blockBind = uboBlockBinding;
//  mapping.size = sizeof(ModelViewProjection);
//  return mapping;
//}