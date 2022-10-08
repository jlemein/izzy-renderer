#pragma once

#include <cstdint>

namespace izz {

/// @brief id of texture, used to identify texture in texture system. This id does not refer to the id of a GPU allocated texture buffer.
using TextureId = int64_t;

/// @brief id of material, used to identify material in material system. This id does not refer to shader program id.
using MaterialId = int64_t;

using MeshBufferId = int64_t;

}  // namespace izz