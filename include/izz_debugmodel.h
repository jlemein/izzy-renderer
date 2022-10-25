//
// Created by jlemein on 10-01-21.
//
#pragma once

#include <izz.h>
#include "ecs_transform.h"
#include "geo_mesh.h"

namespace izz {

/**!
 * A collection of items needed to render a debug model.
 */
// TODO the data is very related to ECS. Change this to create a higher level
//  wrapper that models a generic model.
struct DebugModel {
  std::vector<izz::Geometry> renderable;
  std::vector<Name> names;
  std::vector<ecs::Transform> transformations;
  bool isWireframe = true;
};

}  // namespace izz
