//
// Created by jlemein on 06-11-20.
//

#ifndef GLVIEWER_ECS_INTERACTABLE_H
#define GLVIEWER_ECS_INTERACTABLE_H

#include <entt/entt.hpp>
#include <entt/entity/registry.hpp>

namespace affx {
namespace ecs
{
class IViewerInteractable
{
 public:
  virtual ~IViewerInteractable() = default;

  virtual void
  init(entt::registry &) = 0;
  virtual void
  update(entt::registry &, float time, float dt) = 0;
  virtual void
  render(const entt::registry &registry) = 0;
};

} // end package
} // end enterprise namespace

#endif  // GLVIEWER_ECS_INTERACTABLE_H
