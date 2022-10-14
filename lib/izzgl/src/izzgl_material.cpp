//
// Created by jeffrey on 1-5-22.
//
#include <GL/glew.h>
#include <izzgl_material.h>
#include <uniform_blinnphongsimple.h>
using namespace izz::gl;

//namespace {
//int getUniformLocation(GLint program, const char* name, const std::string& materialName) {
//  int location = glGetUniformLocation(program, name);
//  if (location == -1) {
//    spdlog::warn(
//        fmt::format("{}: unknown uniform parameter '{}' (material: '{}'). Is parameter present and used in shader?", Material::ID, name, materialName));
//  }
//  return location;
//}
//}  // namespace

void Material::useTextures() const {
  int tid = 0;
  for (auto& [name, texture] : textures) {
    auto l = glGetUniformLocation(programId, name.c_str());

    glActiveTexture(GL_TEXTURE0 + tid);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glUniform1i(texture.location, tid);
    ++tid;
  }
}

void Material::pushUniforms() const {
  for (const auto& uniform : uniformBuffers) {
    const auto& mapping = uniform.second;
    glBindBuffer(GL_UNIFORM_BUFFER, uniform.second.bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, mapping.blockBind, mapping.bufferId);

    // is this needed?
    glUniformBlockBinding(programId, mapping.blockIndex, mapping.blockBind);

    void* buff_ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

    std::memcpy(buff_ptr, mapping.data, mapping.size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }

  pushUnscopedUniforms();
}

void Material::pushUnscopedUniforms() const {
  for (const auto& value : globalUniforms->booleanValues) {
    glUniform1i(value.second->m_location, *reinterpret_cast<GLint*>(value.second->m_data));
  }

  for (const auto& value : globalUniforms->intValues) {
    glUniform1i(value.second->m_location, *reinterpret_cast<int*>(value.second->m_data));
  }

  for (const auto& value : globalUniforms->intArrayValues) {
    glUniform1iv(value.second->m_location, value.second->m_length, reinterpret_cast<GLint*>(value.second->m_data));
  }

  for (const auto& value : globalUniforms->floatValues) {
    spdlog::warn("Setting {} to {} (loc: {})", value.first, *(float*)value.second->m_data, value.second->m_location);
    glUniform1f(value.second->m_location, *reinterpret_cast<GLfloat*>(value.second->m_data));
  }

  for (const auto& value : globalUniforms->floatArrayValues) {
    glUniform1fv(value.second->m_location, value.second->m_length, reinterpret_cast<GLfloat*>(value.second->m_data));
  }
}
//
//void Material::addUniformBool(std::string paramName, bool value) {
//  spdlog::debug("Material {}: adding uniform Bool: {}", name, paramName);
//  auto location = getUniformLocation(programId, paramName.c_str(), paramName);
//  if (location != GL_INVALID_INDEX) {
//    UniformProperty* property = globalUniforms->addBoolean(paramName, value, location);
//    m_allUniforms[paramName] = property;
//  }
//}
//
//void Material::addUniformInt(std::string paramName, int value) {
//  spdlog::debug("Material {}: adding uniform int: {}", name, paramName);
//  auto location = getUniformLocation(programId, paramName.c_str(), name);
//  m_allUniforms[paramName] = globalUniforms->addInt(paramName, value, location);
//}
//
//void Material::addUniformFloat(std::string paramName, float value) {
//  spdlog::debug("Material {}: adding uniform float: {}", name, paramName);
//  auto location = getUniformLocation(programId, paramName.c_str(), name);
//  m_allUniforms[paramName] = globalUniforms->addFloat(paramName, value, location);
//}
//
//void Material::addUniformFloatArray(std::string paramName, const std::vector<float>& value) {
//  spdlog::debug("Material {}: adding uniform float array: {}", name, paramName);
//  auto location = getUniformLocation(programId, paramName.c_str(), name);
//  m_allUniforms[paramName] = globalUniforms->addFloatArray(paramName, value, location);
//}