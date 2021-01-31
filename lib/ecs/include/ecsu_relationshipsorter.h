//
// Created by jlemein on 10-01-21.
//

#ifndef GLVIEWER_ECSU_RELATIONSHIPSORTER_H
#define GLVIEWER_ECSU_RELATIONSHIPSORTER_H

#include <entt/fwd.hpp>

namespace affx {
namespace ecsu {

/**!
 * Sorts the entities in the specified registry that have a Relationship
 * component in in a breadth first manner.
 */
class RelationshipSorter {
public:
  static void sort(entt::registry &registry);
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_ECSU_RELATIONSHIPSORTER_H
