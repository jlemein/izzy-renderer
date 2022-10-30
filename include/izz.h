#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <izz_boundingbox.h>
#include <izz_relationship.h>

namespace izz {

/// @brief id of texture, used to identify texture in texture system. This id does not refer to the id of a GPU allocated texture buffer.
using TextureId = int64_t;

/// @brief id of material, used to identify material in material system. This id does not refer to shader program id.
using MaterialId = int64_t;
static const inline MaterialId MATERIAL_UNDEFINED = -1;

using VertexBufferId = int64_t;

struct Name {
  std::string name;
};

/// @brief a geometry is an entity that contains a definition of vertices associated with material information.
/// That means, a geometry could be rendered, depending on the assigned renderer (i.e. forward, deferred).
struct Geometry {
  MaterialId materialId{-1};
  VertexBufferId vertexBufferId{-1};
  BoundingBox aabb; /// @brief bounds of the local geometry. Does not take into account world transformations.
};

}  // namespace izz