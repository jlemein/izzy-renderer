//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <cstdint>

namespace izz {
namespace gl {

constexpr void* BUFFER_OFFSET(unsigned int offset) {
  uint8_t* pAddress = 0;
  return pAddress + offset;
}

/**
 * Abstracts away a frame buffer object.
 */
//struct Framebuffer {
//  GLuint fbo;
//  GLuint sreenQuadVbo {0U};
//};
//
//class FramebufferFactory {
// public:
//  static void CreateDefaultFramebuffer();
//};
};
};
