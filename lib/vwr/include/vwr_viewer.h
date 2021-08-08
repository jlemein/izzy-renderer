// Copyright 2020 Jeffrey Lemein
#pragma once

#include <ecs_firstpersoncontrolsystem.h>
#include <ecs_rendersubsystem.h>
#include <vwr_viewerextension.h>

#include <list>
#include <memory>
#include <string>

#include <ecsg_scenegraphentity.h>
#include <entt/entt.hpp>
namespace lsw {

namespace res {
class ResourceManager;
}

namespace ecs {
class TransformSystem;
class RenderSystem;
class CameraSystem;
class DebugSystem;
class IViewerExtension;
} // namespace ecs

namespace ecsg {
class SceneGraph;
} // namespace ecsg

namespace io {
class InputSystem;
} // namespace io

namespace viewer {
class WindowInputListener;

typedef struct WindowHandle {
} WindowHandle;

struct DisplayDetails {
  WindowHandle *window{nullptr};
  std::string shadingLanguage{""};
  std::string shadingLanguageVersion{""};
  int windowWidth, windowHeight;
};

class Viewer {
public:
  typedef entt::registry SceneGraph;

  /**!
   * A viewer needs a scene graph and a resource manager to operate.
   * @param sceneGraph Contains the scene composition to render.
   * @param resourceManager Used to deallocate objects. (possible viewer does
   * not need this data).
   */
  Viewer(std::shared_ptr<ecsg::SceneGraph> sceneGraph,
         std::shared_ptr<ecs::RenderSystem> renderSystem,
         std::shared_ptr<res::ResourceManager> resourceManager);

  ~Viewer();

  ecsg::SceneGraph &getSceneGraph();

  DisplayDetails getDisplayDetails();

  void initialize();
  int run();

  void setWindowSize(unsigned int width, unsigned int height);
  void setTitle(const std::string &title);

  void setActiveCamera(ecsg::SceneGraphEntity cameraEntity);

  void registerExtension(std::shared_ptr<ecs::IViewerExtension> interactable);
  void registerRenderSubsystem(
      std::shared_ptr<ecs::IRenderSubsystem> renderSubsystem);

private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  std::shared_ptr<res::ResourceManager> m_resourceManager;
  entt::registry &m_registry;

  std::list<std::shared_ptr<ecs::IViewerExtension>> m_viewerExtensions;

  std::shared_ptr<ecs::RenderSystem> m_renderSystem;
  std::shared_ptr<ecs::TransformSystem> m_transformSystem;
  std::shared_ptr<ecs::CameraSystem> m_cameraSystem;
  std::shared_ptr<ecs::DebugSystem> m_debugSystem;
  std::shared_ptr<io::InputSystem> m_inputSystem;
  std::shared_ptr<ecs::FirstPersonMovementSystem> m_firstPersonSystem;

  std::shared_ptr<WindowInputListener> m_genericInputListener;

  DisplayDetails m_displayDetails;
  const std::string m_title = "Hello LSW Renderer";
};

} // namespace viewer
} // namespace lsw
