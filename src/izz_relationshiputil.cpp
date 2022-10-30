#include <izz_relationship.h>
#include <izz_relationshiputil.h>
#include "izz_scenegraphentity.h"
using namespace izz;

void RelationshipUtil::MakeChild(entt::registry& registry, entt::entity parent, entt::entity child) {
  if (!registry.all_of<Relationship>(parent)) {
    registry.emplace<Relationship>(parent);
  }
  if (!registry.all_of<Relationship>(child)) {
    registry.emplace<Relationship>(child);
  }

  auto& parentRelationship = registry.get<Relationship>(parent);
  auto& childRelationship = registry.get<Relationship>(child);

  childRelationship.depth = parentRelationship.depth + 1;
  parentRelationship.children.push_back(child);
  childRelationship.parent = parent;
}

void RelationshipUtil::MakeChild(entt::registry& registry, SceneGraphEntity& parent, SceneGraphEntity& child) {
  return MakeChild(registry, parent.handle(), child.handle());
}

void RelationshipUtil::MakeChildren(entt::registry& registry, entt::entity parent, const std::vector<entt::entity>& children) {
  for (auto child : children) {
    MakeChild(registry, parent, child);
  }
}