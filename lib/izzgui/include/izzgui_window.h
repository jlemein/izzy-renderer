#pragma once

#include <ecs_firstpersoncontrolsystem.h>
#include <izzgui_iwindowextension.h>

#include <list>
#include <memory>
#include <string>

#include <izzgui_guisystem.h>
#include <entt/entt.hpp>
#include "izz_scenegraphentity.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace izz {
class Izzy;

namespace anim {
class AnimationSystem;
}

namespace ecs {
class TransformSystem;
class CameraSystem;
}  // namespace ecs

namespace io {
class InputSystem;
}  // namespace io

namespace gl {
class RenderSystem;
}

class ResourceManager;
class DebugSystem;
class EntityFactory;

namespace gui {
class WindowInputListener;

typedef struct WindowHandle {
} WindowHandle;

struct DisplayDetails {
  WindowHandle* window{nullptr};
  std::string shadingLanguage{""};
  std::string shadingLanguageVersion{""};
  int windowWidth{800}, windowHeight{600};
};

class Window {
 public:
  /**!
   * A viewer needs a scene graph and a resource manager to operate.
   * @param izzy Collection of systems of izzy.
   */
  Window(izz::Izzy& izzy);

  ~Window();

  izz::EntityFactory& getSceneGraph();

  DisplayDetails getDisplayDetails();

  /**
   * Initializes the windowing system, so that there
   */
  void initializeContext();
  void initialize();
  int run();

  void setWindowSize(unsigned int width, unsigned int height);
  void setTitle(const std::string& title);

  // callbacks
  void onWindowResize(int width, int height);

 private:
  std::shared_ptr<izz::EntityFactory> m_sceneGraph;
  std::shared_ptr<izz::ResourceManager> m_resourceManager{nullptr};
  std::shared_ptr<gui::GuiSystem> m_guiSystem{nullptr};
  std::shared_ptr<izz::anim::AnimationSystem> m_animationSystem;
  entt::registry& m_registry;

  std::shared_ptr<gl::RenderSystem> m_renderSystem;
  std::shared_ptr<izz::ecs::TransformSystem> m_transformSystem;
  std::shared_ptr<izz::ecs::CameraSystem> m_cameraSystem;
  std::shared_ptr<izz::DebugSystem> m_debugSystem;
  std::shared_ptr<izz::io::InputSystem> m_inputSystem;
  std::shared_ptr<izz::ecs::FirstPersonMovementSystem> m_firstPersonSystem;

  std::shared_ptr<WindowInputListener> m_genericInputListener;

  DisplayDetails m_displayDetails;
  std::string m_title = "Izzy Renderer";
};

}  // namespace gui
}  // namespace izz
