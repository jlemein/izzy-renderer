//
// Created by jlemein on 13-01-21.
//

#ifndef GLVIEWER_ECS_SCENEGRAPHENTITY_H
#define GLVIEWER_ECS_SCENEGRAPHENTITY_H

#include <memory>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace affx {
namespace ecs {

class SceneGraph;

/**!
 * @brief A scene graph entity by definition has a
 * transform, relationship and name component.
 */
class SceneGraphEntity {
public:
  SceneGraphEntity(entt::registry& owner, entt::entity handle);

  entt::entity id();
  entt::entity handle();

  template <typename Component>  Component& add();
  template <typename Component>  Component& add(Component&&);
  template <typename Component>  Component& get();


  /**!
   * Sets the name of the scene graph entity.
   * @param name Name for this scene graph entity.
   */
  void setName(std::string name);

  /**!
   * @returns the name of the scene graph entity, which is a readable name and
   * not guaranteed to be unique in the scene.
   */
  std::string getName() const;

  /**!
   * Sets the local transformation matrix for this scene graph entity.
   * The local transformation matrix affects the transformation relative to
   * it's parent scene graph entity.
   * @param transform Local transformation matrix.
   */
  void setTransform(glm::mat4 transform);

  const glm::mat4& getTransform() const;

  /**!
   * Removes this entity from the parent's list of children.
   * Also it removes the parent of this entity, so that this entity becomes
   * a node without a parent, aka. it becomes a root node. There can be multiple
   * root nodes in the scene graph.
   */
  void detachFromParent();
  void attachToParent(SceneGraphEntity& parent);

  std::vector<SceneGraphEntity*> getChildren();

  /**!
   * @returns a pointer to the parent entity of this entity, if it exists.
   * If there is no parent entity, then nullptr is returned.
   */
  SceneGraphEntity* getParent();

  void addChild(SceneGraphEntity child);
//  void addChildren(SceneGraphEntity&... children);
  void removeChild(SceneGraphEntity child);
  void removeChildren();

//  void addSibling(SceneGraphEntity& sibling);

private:
  entt::registry& m_registry;
  entt::entity m_handle;
};

// ==========================================================
// INLINE DEFINITIONS
// ==========================================================
template <typename Component>
Component & SceneGraphEntity::get(){
  return m_registry.get<Component>(handle());
}

template <typename Component>
Component& SceneGraphEntity::add() {
  return m_registry.emplace<Component>(handle());
}

template <typename Component>
Component& SceneGraphEntity::add(Component&& comp) {
  return m_registry.emplace<Component>(handle(), std::forward<Component>(comp));
}

} // end of package
} // end of enterprise
#endif // GLVIEWER_ECS_SCENEGRAPHENTITY_H
