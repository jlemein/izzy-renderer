//
// Created by jlemein on 07-11-20.
//

#ifndef GLVIEWER_VIEWER_RENDERSYSTEM_H
#define GLVIEWER_VIEWER_RENDERSYSTEM_H

#include <ecs_renderable.h>
#include <ecs_rendersubsystem.h>
#include <ecs_debugsystem.h>
#include <memory>

#include <glm/glm.hpp>

namespace artifax
{
namespace ecs
{
struct Shader;

/**!
 * Responsible
 */
class RenderSystem
{
 public:
  RenderSystem(entt::registry &registry);

  void init();
  void update(float time, float dt);
  void render();

  // TODO: we need a scene graph so that render system can query all active cameras,
  // lights, etcetera.
  //  void setPerspective(const glm::mat4 &perspective);
  //  void setMovementVector(const glm::vec3 &movementVector);

  // register as part of the render pipeline
  void addSubsystem(std::shared_ptr<IRenderSubsystem> system);
  void removeSubsystem(std::shared_ptr<IRenderSubsystem> system);

 private:
  entt::registry &m_registry;

  void updateCamera(Renderable &renderable, float time, float dt);

  std::list<std::shared_ptr<IRenderSubsystem>> m_renderSubsystems;
  ecs::DebugSystem m_debugSystem;
};

}  // namespace ecs
}  // namespace artifax

#endif  // GLVIEWER_VIEWER_RENDERSYSTEM_H
