//
// Created by jeffrey on 28-10-22.
//
#include <izzgl_shadersource.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <regex>
using namespace izz::gl;

namespace {
/// @brief Retrieves the referenced file path, given an include directivestatement.
/// @details Includes are recognized if the path is between quotes ("") or between angle brackets (<>).
////
/// @param statement Statement containing include directive with the file path. Example: #include <hello.glsl>
/// @return the file path retrieved from the include statement;
/// @throws std::runtime_error if the path could not be retrieved.
///
std::filesystem::path getIncludeFilePath(std::string statement) {
  std::regex rgx("(<.+>)|('.+')|(\".+\")");
  std::smatch match;
  if (std::regex_search(statement, match, rgx)) {
    std::string file = match[0];
    std::filesystem::path path = file.substr(1, file.size() - 2);
    return path;
  }
  throw std::runtime_error(fmt::format("Could not derive include file, from include statement: {}", statement));
}

int getOffsetToFirstLineNotStartingWithDirective(const std::vector<std::string>& lines) {
  // find location to add defines
  auto insertIt = lines.begin();
  int offset = 0;
  for (; insertIt != lines.end(); ++offset, ++insertIt) {
    auto trimmedLine = boost::algorithm::trim_left_copy(*insertIt);
    if (!trimmedLine.starts_with("#")) {
      break;
    }
  }
  return offset;
}
}  // namespace

std::filesystem::path izz::gl::ShaderContext::ResolvePath(std::filesystem::path path, ShaderContext* context) {
  if (context && path.is_absolute()) {
    path = context->root / path.string().substr(1);
  }
  return path;
}

ShaderSource::ShaderSource() {}

ShaderSource::ShaderSource(ShaderContext* context)
  : m_context{context} {}

const SourceCode& ShaderSource::getSourceCode() const {
  return m_combinedSourceLines;
}

const SourceCodeLineLength& ShaderSource::getSourceCodeLineLengths() const {
  return m_combinedSourceLengths;
}

void ShaderSource::setContext(ShaderContext* pContext) {
  m_context = pContext;
}

const BinaryShaderData& ShaderSource::getBinarySourceCode() const {
  return m_binaryShader;
}

void ShaderSource::resolveContext() {
  if (m_context) {
    auto offset = getOffsetToFirstLineNotStartingWithDirective(m_source);
    for (const auto& flag : m_context->compileConstants.flags) {
      m_source.insert(m_source.begin() + offset, fmt::format("#define {}\n", flag));
    }
    m_source.insert(m_source.begin() + offset, "// Compile constants inserted by Izzy renderer\n");
  }

  for (const auto& s : m_source) {
    m_combinedSourceLines.emplace_back(s.c_str());
    m_combinedSourceLengths.emplace_back(s.size());
  }
}

void ShaderSource::readFile(std::filesystem::path path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("failed to read file '{}'", std::filesystem::absolute(path).string()));
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.starts_with("#include")) {
      auto includePath = getIncludeFilePath(line);
      includePath = ShaderContext::ResolvePath(includePath, m_context);

      // Read file referenced file in #include directive.
      ShaderSource includedSource(m_context);
      includedSource.readFile(includePath);
      for (const auto& includedLine : includedSource.m_source) {
        m_source.emplace_back(includedLine);
      }
    } else {
      m_source.push_back(line + "\n");
    }
  }

  file.close();
}

void ShaderSource::readBinaryFile(std::filesystem::path filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("failed to read binary file '{}'", filename.string()));
  }

  size_t fileSize = (size_t)file.tellg();
  m_binaryShader.reserve(fileSize);

  file.seekg(0);
  file.read(m_binaryShader.data(), fileSize);
  file.close();
}