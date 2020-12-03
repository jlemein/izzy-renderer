//
// Created by jlemein on 11-11-20.
//

#ifndef GLVIEWER_ECS_RELATIONSHIP_H
#define GLVIEWER_ECS_RELATIONSHIP_H

#include <entt/entt.hpp>
#include <vector>

struct Relationship {
  entt::entity parent;
  std::vector<entt::entity> children;

  inline bool hasParent() const {
    return parent != entt::null;
  }

  inline bool hasChildren() const {
    return !children.empty();
  }
};
#endif // GLVIEWER_ECS_RELATIONSHIP_H
