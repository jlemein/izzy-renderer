//
// Created by jlemein on 10-01-21.
//
#pragma once

#include "gl_renderable.h"
#include "ecs_transform.h"
#include "ecs_name.h"
#include "geo_mesh.h"

namespace lsw {
namespace ecs {

/**!
 * A collection of items needed to render a debug model.
 */
 // TODO the data is very related to ECS. Change this to create a higher level
 //  wrapper that models a generic model.
struct DebugModel {
  std::vector<izz::gl::Renderable> renderable;
  std::vector<izz::gl::MaterialDescription> material;
  std::vector<Name> names;
  std::vector<geo::Mesh> mesh;
  std::vector<ecs::Transform> transformations;
//  geo::Curve curve;
};

} // end of package
} // end of enterprise
