// Copyright 2020 Jeffrey Lemein
#pragma once

#include <ecs_firstpersoncontrolsystem.h>
#include <glrs_rendersubsystem.h>
#include <vwr_viewerextension.h>

#include <list>
#include <memory>
#include <string>

#include <ecsg_scenegraphentity.h>
#include <entt/entt.hpp>
#include <gui_guisystem.h>

namespace lsw {

namespace anim {
class AnimationSystem;
}

namespace glrs {
class RenderSystem;
}

class ResourceManager;

namespace ecs {
class TransformSystem;
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
         std::shared_ptr<glrs::RenderSystem> renderSystem,
         std::shared_ptr<ResourceManager> resourceManager,
         std::shared_ptr<gui::GuiSystem> guiSystem = nullptr);

  ~Viewer();

  ecsg::SceneGraph &getSceneGraph();

  DisplayDetails getDisplayDetails();

  void initialize();
  int run();

  void setWindowSize(unsigned int width, unsigned int height);
  void setTitle(const std::string &title);

  void setActiveCamera(ecsg::SceneGraphEntity cameraEntity);



private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  std::shared_ptr<ResourceManager> m_resourceManager;
  std::shared_ptr<gui::GuiSystem> m_guiSystem {nullptr};
  std::shared_ptr<anim::AnimationSystem> m_animationSystem;
  entt::registry &m_registry;

  std::shared_ptr<glrs::RenderSystem> m_renderSystem;
  std::shared_ptr<ecs::TransformSystem> m_transformSystem;
  std::shared_ptr<ecs::CameraSystem> m_cameraSystem;
  std::shared_ptr<ecs::DebugSystem> m_debugSystem;
  std::shared_ptr<io::InputSystem> m_inputSystem;
  std::shared_ptr<ecs::FirstPersonMovementSystem> m_firstPersonSystem;

  std::shared_ptr<WindowInputListener> m_genericInputListener;

  DisplayDetails m_displayDetails;
  std::string m_title = "Hello LSW Renderer";
};

} // namespace viewer
} // namespace lsw
