#ifndef INCLUDED_VIEWER_TEXTURE_H_
#define INCLUDED_VIEWER_TEXTURE_H_

#include <string>

namespace izz {
namespace ecs {

/**!
 * @brief Exposed component for textures
 */
struct Texture {
  std::string name;
    // parameter name in shader

  uint32_t glTextureId;
};

} // end of package
} // end of enterprise

#endif // INCLUDED_VIEWER_TEXTURE_H_