//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>
#include <cstdint>
#include <iostream>

namespace izz {
namespace gl {

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

void checkError(const char* name) {
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error occurred for " << name << ": " << err << std::endl;
  }
}

}  // namespace gl
}  // namespace izz
