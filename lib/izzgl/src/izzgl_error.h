//
// Created by jeffrey on 24-9-22.
//
#pragma once

#include <GL/glew.h>
#include <fmt/format.h>
#include <iostream>
#include <unordered_map>

namespace izz::gl {

inline void checkError(const char* name) {
  static std::unordered_map<int, std::string> errorMap{
      {GL_INVALID_ENUM, "Invalid enum"}, {GL_INVALID_OPERATION, "Invalid operation"}, {GL_INVALID_FRAMEBUFFER_OPERATION, "Invalid framebuffer operation"}};

  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    auto msg = fmt::format("OpenGL error {} ({}): {}", err, errorMap.contains(err) ? errorMap.at(err) : "Unknown", name);
    std::cerr << msg << std::endl;
    throw std::runtime_error(msg);
  }
}

}  // namespace izz::gl
