//
// Created by jlemein on 13-01-21.
//
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace izz {

class EntityFactory;

/**!
 * @brief A scene graph entity by definition has a transform, relationship and
 * name component.
 *
 * The scene graph entity contains limited functionality to perform operations
 * on those core components. In case you want to adjust other components, use
 * the generic add and get component functionality.
 */
class SceneGraphEntity {
 public:
  SceneGraphEntity(entt::registry& owner, entt::entity handle);

  uint64_t id() const;
  entt::entity handle();

  operator entt::entity() const {
    return m_handle;
  }

  /**!
   * @returns the name of the scene graph entity, which is a readable name and
   * not guaranteed to be unique in the scene.
   */
  std::string getName() const;

  /**!
   * Sets the name of the scene graph entity.
   * @param name Name for this scene graph entity.
   */
  void setName(std::string name);

  /**!
   * @returns a const reference to the local transform matrix.
   */
  const glm::mat4& getTransform() const;

  /**
   * @returns a reference to the local transform matrix, eligbible to be edited.
   */
  glm::mat4& getTransform();

  /**!
   * Sets the local transformation matrix for this scene graph entity.
   * The local transformation matrix affects the transformation relative to
   * it's parent scene graph entity.
   * @param transform Local transformation matrix.
   */
  void setTransform(const glm::mat4& transform);

  void translate(const glm::vec3& translate);

  template <typename Component>
  decltype(auto) add();

  template <typename Component>
  decltype(auto) add(Component&&);
  template <typename Component>
  decltype(auto) add(const Component&);
  template <typename Component>
  Component& get();

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
Component& SceneGraphEntity::get() {
#ifndef NDEBUG
  if (m_registry.all_of<Component>(handle())) {
#endif
    return m_registry.get<Component>(handle());
#ifndef NDEBUG
  } else {
    throw std::runtime_error("Does not have component");
  }
#endif
}

template <typename Component>
decltype(auto) SceneGraphEntity::add() {
  return m_registry.emplace_or_replace<Component>(handle());
}

template <typename Component>
decltype(auto) SceneGraphEntity::add(Component&& comp) {
  return m_registry.emplace_or_replace<Component>(handle(), std::forward<Component>(comp));
}

template <typename Component>
decltype(auto) SceneGraphEntity::add(const Component& comp) {
  return m_registry.emplace_or_replace<Component>(handle(), comp);
}

}  // namespace izz