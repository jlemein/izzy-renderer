//
// Created by jlemein on 02-12-21.
//
#include <GL/glew.h>
#include <cstdint>
#include <iostream>

namespace izz {
namespace gl {

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

}  // namespace gl
}  // namespace izz
