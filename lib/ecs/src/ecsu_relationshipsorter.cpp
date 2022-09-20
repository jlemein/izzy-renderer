//
// Created by jlemein on 10-01-21.
//
#include <ecs_relationship.h>
#include <ecsu_relationshipsorter.h>
#include <iostream>

using namespace izz::ecsu;

bool RelationshipSorter::Compare(const entt::registry& registry, const entt::entity lhs, const entt::entity rhs) {
  const auto &clhs = registry.get<ecs::Relationship>(lhs);
  const auto &crhs = registry.get<ecs::Relationship>(rhs);

  return Compare(clhs, crhs);
}

bool RelationshipSorter::Compare(const ecs::Relationship& lhs, const ecs::Relationship& rhs) {
  bool isLeftSibling = lhs.parent == rhs.parent && &lhs < &rhs;
  bool isParentEarlier = (lhs.parent != rhs.parent) && (lhs.depth == rhs.depth) && (&lhs < &rhs);
  bool isLeftHigherInHierarchy = lhs.depth < rhs.depth;

  // the order when different parents
  return isLeftHigherInHierarchy || isLeftSibling || isParentEarlier;
}
