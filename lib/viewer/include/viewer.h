// Copyright 2020 Jeffrey Lemein
#pragma once

#include <ecs_firstpersoncontrolsystem.h>
#include <ecs_interactable.h>
#include <ecs_rendersubsystem.h>

#include <list>
#include <memory>
#include <string>

#include <entt/entt.hpp>

namespace artifax {

namespace ecs {
class TransformSystem;
class RenderSystem;
class CameraSystem;
class DebugSystem;
class IViewerInteractable;
} // namespace ecs

namespace io {
class InputSystem;
} // namespace io

namespace viewer {
class WindowInputListener;

class Viewer {
public:
  typedef entt::registry SceneGraph;

  Viewer();
  ~Viewer();

  SceneGraph &getRegistry();

  int run();

  void
  registerExtension(std::shared_ptr<ecs::IViewerInteractable> interactable);
  void registerRenderSubsystem(
      std::shared_ptr<ecs::IRenderSubsystem> renderSubsystem);

private:
  void init();

  entt::registry m_registry;
  std::list<std::shared_ptr<ecs::IViewerInteractable>> mmInteractables;

  std::shared_ptr<ecs::RenderSystem> m_renderSystem;
  std::shared_ptr<ecs::TransformSystem> m_transformSystem;
  std::shared_ptr<ecs::CameraSystem> m_cameraSystem;
  std::shared_ptr<ecs::DebugSystem> m_debugSystem;
  std::shared_ptr<io::InputSystem> m_inputSystem;
  std::shared_ptr<ecs::FirstPersonMovementSystem> m_firstPersonSystem;

  std::shared_ptr<WindowInputListener> m_genericInputListener;
};

} // namespace viewer
} // namespace artifax
