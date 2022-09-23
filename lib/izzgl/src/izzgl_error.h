//
// Created by jeffrey on 24-9-22.
//
#pragma once

#include <GL/glew.h>
#include <iostream>
#include <unordered_map>

namespace izz::gl {

inline void checkError(const char* name) {
  static std::unordered_map<int, std::string> errorMap{{GL_INVALID_ENUM, "Invalid enum"}, {GL_INVALID_OPERATION, "Invalid operation"}};

  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error " << err;
    if (errorMap.contains(err)) {
      std::cerr << ": " << errorMap.at(err);
    }
    std::cerr << " [" << name << "]\n";
  }
}

}  // namespace izz::gl
