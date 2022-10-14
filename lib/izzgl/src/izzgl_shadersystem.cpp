#include <GL/glew.h>
#include <izzgl_shaderlogprettifier.h>
#include <izzgl_shadersystem.h>
#include <fstream>
#include "izzgl_material.h"
using namespace izz::gl;

namespace {
std::vector<std::string> readFile(const std::filesystem::path& filename) {
  // TODO: check for empty file name and give clear error message

  std::vector<std::string> lines{};
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      lines.push_back(line + "\n");
    }

    file.close();
  } else {
    throw std::runtime_error(fmt::format("failed to read file '{}'", std::filesystem::absolute(filename).string()));
  }

  return lines;
}

std::vector<char> readSpirvFile(const std::filesystem::path& filename) {
  // TODO: check for empty file name and give clear error message

  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("failed to read file '{}'", filename.string()));
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

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

// trim from start (in place)
static inline void ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
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

void ShaderCompiler::setCompileConstant(std::string key, std::string value) {
  m_compileConstants.settings[key] = value;
}

void ShaderCompiler::enableCompileConstant(std::string key) {
  m_compileConstants.flags.insert(key);
}

void ShaderCompiler::addCompileConstants(std::vector<std::string>& lines) {
  // find location to add defines
  auto insertIt = lines.begin();
  int offset = 0;
  for (; insertIt != lines.end(); ++offset, ++insertIt) {
    ltrim(*insertIt);
    if (!insertIt->starts_with("#")) {
      break;
    }
  }

  for (const auto& flag : m_compileConstants.flags) {
    lines.insert(lines.begin() + offset, fmt::format("#define {}\n", flag));
  }
  lines.insert(lines.begin() + offset, "// Compile constants inserted by Izzy renderer\n");
}

ShaderCompiler::Program ShaderCompiler::compileShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
  std::vector<const char*> vertexShaderSource{}, fragmentShaderSource{};
  std::vector<GLint> vertexShaderSourceLengths{}, fragmentShaderSourceLengths{};

  // Read vertex shader line by line
  auto vertexShaderLines = readFile(vertexShaderPath);
  addCompileConstants(vertexShaderLines);

  for (const auto& s : vertexShaderLines) {
    vertexShaderSource.emplace_back(s.c_str());
    vertexShaderSourceLengths.emplace_back(s.size());
  }

  // Read fragment shader line by line
  auto fragmentShaderLines = readFile(fragmentShaderPath);
  addCompileConstants(fragmentShaderLines);

  for (const auto& s : fragmentShaderLines) {
    fragmentShaderSource.emplace_back(s.c_str());
    fragmentShaderSourceLengths.emplace_back(s.size());
  }

  int vsCompiled = 0, fsCompiled = 0;
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, vertexShaderLines.size(), vertexShaderSource.data(), vertexShaderSourceLengths.data());
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);

  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, fragmentShaderLines.size(), fragmentShaderSource.data(), fragmentShaderSourceLengths.data());
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fsCompiled);

  if (!fsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile fragment shader '{}'\n{}", fragmentShaderPath, getShaderLog(fragment_shader));
  }

  if (!vsCompiled) {
    spdlog::log(spdlog::level::err, "Failed to compile vertex shader '{}'\n{}", vertexShaderPath, getShaderLog(vertex_shader));
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
    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}", vertexShaderPath, fragmentShaderPath);
    printLinkingLog(program);
  }

  return static_cast<ShaderCompiler::Program>(program);
}

ShaderCompiler::Program ShaderCompiler::compileSpirvShader(const std::string& vertexShader, const std::string& fragmentShader) {
  if (!this->hasBinaryShaderSupport()) {
    throw std::runtime_error("Cannot compile material into shader program: no binary shader support for this machine");
  }

  auto vertexShaderBuffer = readFile(vertexShader);
  auto fragmentShaderBuffer = readFile(fragmentShader);

  int vsCompiled = 0, fsCompiled = 0;
  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderBinary(1, &vertex_shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertexShaderBuffer.data(), vertexShaderBuffer.size());
  glSpecializeShaderARB(vertex_shader, "main", 0, 0, 0);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vsCompiled);

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
    spdlog::log(spdlog::level::err, "Linking failed for vs: {}, fs: {}", vertexShader, fragmentShader);
    printLinkingLog(program);
  }

  return static_cast<ShaderCompiler::Program>(program);
}