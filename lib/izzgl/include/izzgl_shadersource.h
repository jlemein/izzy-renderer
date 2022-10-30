//
// Created by jeffrey on 28-10-22.
//
#pragma once

#include <GL/glew.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include "izz_capabilities.h"

namespace izz {
namespace gl {

using CompileConstants = Capabilities;
using BinaryShaderData = std::vector<char>;
using SourceCode = std::vector<const char*>;
using SourceCodeLineLength = std::vector<int>;

/**
 * Shader context represents metadata used by the shader compiler to compile a shader.
 * A shader context determines the root directory for shader includes and also contains
 * a list of compile flags, that should be enabled in the shader.
 */
struct ShaderContext {
  std::filesystem::path root = "/";   /// @brief The root directory used to resolve include statements.
  CompileConstants compileConstants;  /// @brief Flags or constants that need to be added or replaces in the shader source code,
                                      /// whenever a call to \see resolveContext is made.

  /**
   * Resolves a provided path given a context object. The context object contains a root directory which is used to offset the provided path argument.
   * @param path The input path to resolve, given the shader context.
   * @param context The shader context.
   * @return The path with the context taken into account. If no context is specified, the same path is returned as provided as input.
   */
  static std::filesystem::path ResolvePath(std::filesystem::path path, ShaderContext* context);
};

/**
 * Shader source represents the source code of a shader file. In it's simplest form it corresponds to simple source code lines.
 * However, shader files may contain numerous includes, define statements and special tags to facilitate writing shader functionality.
 * The abstraction is needed to handle operations on the shader source code, such as replacing special tags with engine specific values.
 */
class ShaderSource {
 private:
  ShaderContext* m_context{nullptr};               /// @brief Shader context, defining the root directory and compile constant flags.
  std::vector<std::string> m_source;               /// @brief Plain text lines as read from the provided files, via \see readFile.
  std::vector<char> m_binaryShader;                /// @brief untouched binary contents loaded by call to \see readBinaryFile
  std::vector<const char*> m_combinedSourceLines;  /// @brief Resolved source code, resulting after a call to \see resolveContext.
  std::vector<GLint> m_combinedSourceLengths;      /// @brief Line lengths of the attributes in \see m_combinedSourceLines.

 public:
  /// @brief constructor
  ShaderSource();

  /**
   * Constructor
   * @param context The shader context.
   */
  ShaderSource(ShaderContext* context);

  /**
   * Reads the shader file.
   * @param path Path to the shader file.
   */
  void readFile(std::filesystem::path path);

  /**
   * Reads a binary shader file.
   * @param path The path to the binary shader file (e.g. a spirv file).
   */
  void readBinaryFile(std::filesystem::path path);

  /**
   * Sets the shader context. Setting the context to a nullptr is equivalent to removing the context.
   * The shader context contains information about the includes and define variables.
   *
   * @param pContext Shader context.
   */
  void setContext(ShaderContext* pContext);

  /**
   * If the shader code loaded is binary data, then you can use this call to retrieve the binary data.
   * You don't need to call resolve before getting the binary data.
   *
   * @return The content from the shader file loaded.
   */
  const BinaryShaderData& getBinarySourceCode() const;

  /**
   * Returns a list of souce code lines for the shader. Make sure to call \see resolveContext before calling this function.
   *
   * @return a list of source code lines, to be passed into the OpenGL shader compiler, using the command glShaderSource.
   */
  const SourceCode& getSourceCode() const;

  /**
   * Returns a list of source code line lengths for the current shader source code.
   * The i-th element indicates the length of the i-th element of source code line (returned by calling \see getSourceCode).
   * Make sure to call \see resolveContext before calling this function.
   *
   * @return a list of source code line lengths, to be passed into the OpenGL shader compiler, using the command glShaderSource.
   */
  const SourceCodeLineLength& getSourceCodeLineLengths() const;

  /**
   * Resolves the shader code. Parameters are replaced with their values as described by the shader context.
   * Results in a data structure that is updated with the actual source code that can be fed into the shader compiler.
   * Retrieve the resulting source code and line lengths using the getters.
   */
  void resolveContext();
};

}  // namespace gl
}  // namespace izz