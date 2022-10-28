//
// Created by jeffrey on 25-10-22.
//

#ifndef RENDERER_IZZGL_SHADERINCLUDECRAWLER_H
#define RENDERER_IZZGL_SHADERINCLUDECRAWLER_H

#include <filesystem>

void processShaderIncludes(std::filesystem::path directory) {
  for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(directory)) {
    shaderCompiler.addInclude()
  }
}

#endif  // RENDERER_IZZGL_SHADERINCLUDECRAWLER_H
