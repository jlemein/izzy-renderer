//
// Created by jlemein on 02-01-21.
//
#pragma once
#include <entt/fwd.hpp>
#include <vector>

namespace izz {

/**!
 * @brief Utility methods to set data on a @see Relationship component.
 * It is recommended to set data via the utility methods. They keep the
 * relationship component in a valid state.
 */
struct RelationshipUtil {
 public:
  static void MakeChild(entt::registry& registry, entt::entity parent, entt::entity child);
  static void MakeChildren(entt::registry& registry, entt::entity parent, const std::vector<entt::entity>& children);

  /**!
   * Removes an entity with the relationship component from the hierarchy.
   * It removes the relationship component.
   * If the specified entity is a leaf node, then the node is removed from the parent's list of children.
   * If the specified node is a root node, then all children become independent root nodes.
   * IF the specified node is a parent as well as a child, the children will be attached .
   *
   * Whenever a child is re-attached to the new parent node, the local transform of the 'to be removed' entity is applied to the children.
   * THis makes sure that the hierarchy doesn't change a transform.
   */
  //  static void DetachEntity();
};

}  // namespace izz
