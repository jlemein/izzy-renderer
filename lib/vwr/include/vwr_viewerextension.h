//
// Created by jlemein on 06-11-20.
//

#ifndef GLVIEWER_ECS_INTERACTABLE_H
#define GLVIEWER_ECS_INTERACTABLE_H

#include <entt/entt.hpp>
#include <entt/entity/registry.hpp>

namespace lsw {
namespace viewer {
class Viewer;
}

namespace ecs
{
class IViewerExtension
{
 public:
  virtual ~IViewerExtension() = default;

  /**
   * Place to initialize your system with viewer specific variables.
   * At this point the viewer has initialized a window context, and you can
   * query device metrics.
   */
  virtual void initialize() = 0;

  /**
   * Called every frame to do update behavior. Check for changed state and update
   * the attributes required. Do not render things here.
   * When this method is called the transform system has already run. --> should not right?
   * The order run is based on the order added to the queue.
   *
   * @details
   * It is important to respect the update system. There are two types of updates.
   * * one in which you depend on others, AI that follows an enemy.
   * The AI system runs on
   * * you depend on your own movement and time, for example: animation systems that reflect the current time.
   *
   * update(time) - You can update your position, the transform system will still run afterwards. The transform system
   * only runs to make sure the transformation hierarchy is up-to-date and the lo
   *
   * @param time
   * @param dt
   */
  virtual void update(float time, float dt) = 0;

  virtual void beforeRender() = 0;
  virtual void afterRender() = 0;

  // beforeRender
  // afterRender

  virtual void
  cleanup() {}
};

} // end package
} // end enterprise namespace

#endif  // GLVIEWER_ECS_INTERACTABLE_H
