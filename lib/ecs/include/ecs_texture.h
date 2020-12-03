#ifndef INCLUDED_VIEWER_TEXTURE_H_
#define INCLUDED_VIEWER_TEXTURE_H_

#include <string>

/**!
 * @brief Exposed component for textures
 */
struct Texture {
  uint32_t glTextureId;
  std::string diffuseTextureFilepath;
};

#endif // INCLUDED_VIEWER_TEXTURE_H_