//
// Created by jlemein on 10-01-21.
//

#ifndef GLVIEWER_ECSU_RELATIONSHIPSORTER_H
#define GLVIEWER_ECSU_RELATIONSHIPSORTER_H

#include <entt/fwd.hpp>

namespace izz {
namespace ecs {
struct Relationship;
}
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
  static void Sort(entt::registry &registry);

  static bool Compare(const entt::registry& registry, const entt::entity lhs, const entt::entity rhs);
  static bool Compare(const ecs::Relationship& lhs, const ecs::Relationship& rhs);
};

template <typename T>
void RelationshipSorter::Sort(entt::registry &registry) {
  registry.sort<T>([&registry](const entt::entity lhs, const entt::entity rhs) {
    return Compare(registry, lhs, rhs);
  });
}

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSU_RELATIONSHIPSORTER_H
