#include "izzgl_shadersystem.h"
#include <GL/glew.h>
#include <izzgl_shaderlogprettifier.h>
#include <fstream>
#include <regex>
#include "izzgl_material.h"
#include "izzgl_shadersource.h"
using namespace izz::gl;

namespace {
std::string getShaderLog(GLint shader) {
  GLint reqBufferSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &reqBufferSize);

  std::vector<char> buffer;
  buffer.resize(reqBufferSize);
  glGetShaderInfoLog(shader, reqBufferSize, nullptr, buffer.data());

  std::string log(buffer.begin(), buffer.end());

  return ShaderLogPrettifier().MakePretty(log);
}

void printLinkingLog(GLint program) {
  spdlog::log(spdlog::level::err, "Failed to link shader");

  GLint reqBufferSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &reqBufferSize);

  std::vector<char> buffer;
  buffer.resize(reqBufferSize);
  glGetProgramInfoLog(program, reqBufferSize, nullptr, buffer.data());

  std::string log(buffer.begin(), buffer.end());
  spdlog::error("Program log: {}", log);
}
}  // namespace

ShaderCompiler::ShaderCompiler() {
  // detect binary shader support (on integrated GPU, such as AMD Ryzen 7 5700G, there is no support).
  GLint numberShaderBinaryFormats = 0;
  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &numberShaderBinaryFormats);
  m_hasBinaryShaderSupport = numberShaderBinaryFormats > 0;
}

bool ShaderCompiler::hasBinaryShaderSupport() {
  return m_hasBinaryShaderSupport;
}

ShaderProgram ShaderCompiler::compileShader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath, ShaderContext* pContext) {
  ShaderSource vertexShader(pContext);
  vertexShader.readFile(vertexShaderPath);
  vertexShader.resolveContext();
  const auto& vsSource = vertexShader.getSourceCode();
  const auto& vsSourceLengths = vertexShader.getSourceCodeLineLengths();
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, vsSource.size(), vsSource.data(), vsSourceLengths.data());
  glCompileShader(vertex_shader);
  GLint vsCompiled = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);

  ShaderSource fragmentShader(pContext);
  fragmentShader.readFile(fragmentShaderPath);
  fragmentShader.resolveContext();
  const auto& fsSource = fragmentShader.getSourceCode();
  const auto& fsSourceLengths = fragmentShader.getSourceCodeLineLengths();
  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, fsSource.size(), fsSource.data(), fsSourceLengths.data());
  glCompileShader(fragment_shader);
  GLint fsCompiled = 0;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsCompiled);

  if (!fsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile fragment shader '{}'\n{}", fragmentShaderPath.string(), getShaderLog(fragment_shader));
  }

  if (!vsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile vertex shader '{}'\n{}", vertexShaderPath.string(), getShaderLog(vertex_shader));
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
    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}", vertexShaderPath.string(), fragmentShaderPath.string());
    printLinkingLog(program);
  }

  return static_cast<ShaderProgram>(program);
}

ShaderProgram ShaderCompiler::compileSpirvShader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader) {
  if (!this->hasBinaryShaderSupport()) {
    throw std::runtime_error("Cannot compile material into shader program: no binary shader support for this machine");
  }

  ShaderSource vertexSource;
  vertexSource.readBinaryFile(vertexShader);

  ShaderSource fragmentSource;
  fragmentSource.readBinaryFile(fragmentShader);

  const auto vertexShaderBuffer = vertexSource.getBinarySourceCode();
  int vsCompiled = 0, fsCompiled = 0;
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertexShaderBuffer.data(), vertexShaderBuffer.size());
  glSpecializeShaderARB(vertex_shader, "main", 0, 0, 0);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);

  const auto fragmentShaderBuffer = fragmentSource.getBinarySourceCode();
  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderBinary(1, &fragment_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fragmentShaderBuffer.data(), fragmentShaderBuffer.size());
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
    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}", vertexShader.string(), fragmentShader.string());
    printLinkingLog(program);
  }

  return static_cast<ShaderProgram>(program);
}
