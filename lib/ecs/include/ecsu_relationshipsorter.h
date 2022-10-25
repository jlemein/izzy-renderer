//
// Created by jlemein on 10-01-21.
//
#pragma once

#include <entt/entt.hpp>

namespace izz {
struct Relationship;
namespace ecsu {

/**!
 * Sorts the entities in the specified registry that have a Relationship
 * component in in a breadth first manner.
 */
class RelationshipSorter {
 public:
  /**!
   * Sorts another component based on the relationship component
   * Possibly needs to be moved to another component.
   * A generic sorter class, that takes a comparator.
   * Then this class becomes RelationshipComparator
   * @tparam T
   * @param registry
   */
  template <typename T>
  static void Sort(entt::registry& registry);

  static bool Compare(const entt::registry& registry, const entt::entity lhs, const entt::entity rhs);
  static bool Compare(const izz::Relationship& lhs, const izz::Relationship& rhs);
};

template <typename T>
void RelationshipSorter::Sort(entt::registry& registry) {
  registry.sort<T>([&registry](const entt::entity lhs, const entt::entity rhs) { return Compare(registry, lhs, rhs); });
}

}  // namespace ecsu
}  // namespace izz
