//
// Created by jlemein on 22-02-21.
//

#ifndef RENDERER_GEO_TEXTURE_H
#define RENDERER_GEO_TEXTURE_H

#include <string>
#include <vector>
namespace lsw {
namespace geo {

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

#endif // RENDERER_GEO_TEXTURE_H
