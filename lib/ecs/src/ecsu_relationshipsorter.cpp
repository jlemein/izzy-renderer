//
// Created by jlemein on 10-01-21.
//
#include <ecs_relationship.h>
#include <ecsu_relationshipsorter.h>
#include <iostream>

using namespace artifax::ecsu;

void RelationshipSorter::sort(entt::registry &registry) {
  registry.sort<Relationship>(
      [&registry](const entt::entity lhs, const entt::entity rhs) {
        const auto &clhs = registry.get<Relationship>(lhs);
        const auto &crhs = registry.get<Relationship>(rhs);

        bool isLeftSibling = clhs.parent == crhs.parent && &clhs < &crhs;
        bool isParentEarlier = (clhs.parent != crhs.parent) && (clhs.depth == crhs.depth) && (&clhs < &crhs);
        bool isLeftHigherInHierarchy = clhs.depth < crhs.depth;

        // the order when different parents
        return isLeftHigherInHierarchy || isLeftSibling || isParentEarlier;
      });
}