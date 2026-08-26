//
// Created by jlemein on 10-01-21.
//
#include <ecsu_relationshipsorter.h>
#include <izz_relationship.h>

using namespace izz::ecsu;

bool RelationshipSorter::Compare(const entt::registry& registry, const entt::entity lhs, const entt::entity rhs) {
  const auto &clhs = registry.get<izz::Relationship>(lhs);
  const auto &crhs = registry.get<izz::Relationship>(rhs);

  return Compare(clhs, crhs);
}

bool RelationshipSorter::Compare(const izz::Relationship& lhs, const izz::Relationship& rhs) {
  bool isLeftSibling = lhs.parent == rhs.parent && &lhs < &rhs;
  bool isParentEarlier = (lhs.parent != rhs.parent) && (lhs.depth == rhs.depth) && (&lhs < &rhs);
  bool isLeftHigherInHierarchy = lhs.depth < rhs.depth;

  // the order when different parents
  return isLeftHigherInHierarchy || isLeftSibling || isParentEarlier;
}
