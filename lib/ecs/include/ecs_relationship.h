//
// Created by jlemein on 11-11-20.
//

#pragma once

#include <entt/entt.hpp>
#include <vector>

namespace izz {
namespace ecs {

struct Relationship {
  entt::entity parent {entt::null};
  std::vector<entt::entity> children {};
  int depth {0}; // assume root

  inline bool hasParent() const {
    return parent != entt::null;
  }

  inline bool hasChildren() const {
    return !children.empty();
  }
};

} // enf of package
} // end of enterprise
