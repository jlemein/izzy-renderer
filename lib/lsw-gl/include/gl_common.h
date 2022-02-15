//
// Created by jlemein on 02-12-21.
//
#pragma once

#include <GL/glew.h>
#include <cstdint>
#include <iostream>

namespace izz {
namespace gl {

constexpr void* BUFFER_OFFSET(unsigned int offset);

void checkError(const char* name);

}  // namespace gl
}  // namespace izz
