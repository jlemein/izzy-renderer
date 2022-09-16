//
// Created by jeffrey on 07-11-21.
//
#pragma once

#include <GL/glew.h>
#include "izzgl_material.h"

namespace izz {
namespace gl {

struct ShaderLogPrettifier {
 public:
  std::string MakePretty(const std::string& log) {
    std::string title = "";
    auto b =0;
    if (auto loc = log.find_first_of("\n") != std::string::npos) {
      title = log.substr(b, loc);
      b += loc;
    }

    return log;//title;
  }
};

} // ecs
} // lsw
