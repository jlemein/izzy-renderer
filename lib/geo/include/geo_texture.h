//
// Created by jlemein on 22-02-21.
//
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace lsw {
namespace geo {

struct Rgba8888 {
    uint8_t r, g, b, a;
};

struct Texture {
  std::string path;
  int width {0};
  int height {0};
  int depth {1};
  int channels {0};
  std::vector<uint8_t> data;
};

} // end of package
} // end of enterprise
