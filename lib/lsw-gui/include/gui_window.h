#pragma once

#include <ecs_firstpersoncontrolsystem.h>
#include <gui_iwindowextension.h>

#include <list>
#include <memory>
#include <string>

#include <gui_guisystem.h>
#include <entt/entt.hpp>
#include "izz_scenegraphentity.h"

#include <GLFW/glfw3.h>

namespace lsw {

namespace anim {
class AnimationSystem;
}



class ResourceManager;

namespace ecs {
class TransformSystem;
class CameraSystem;
class DebugSystem;
class IViewerExtension;
}  // namespace ecs

namespace io {
class InputSystem;
}  // namespace io
}  // namespace lsw

namespace izz {
namespace gl {
class RenderSystem;
}
class SceneGraphHelper;

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
  typedef entt::registry SceneGraph;

  /**!
   * A viewer needs a scene graph and a resource manager to operate.
   * @param sceneGraph Contains the scene composition to render.
   * @param resourceManager Used to deallocate objects. (possible viewer does
   * not need this data).
   */
  Window(std::shared_ptr<izz::SceneGraphHelper> sceneGraph, std::shared_ptr<gl::RenderSystem> renderSystem,
         std::shared_ptr<gui::GuiSystem> guiSystem = nullptr);

  ~Window();

  izz::SceneGraphHelper& getSceneGraph();

  DisplayDetails getDisplayDetails();

  void initialize();
  int run();

  void setWindowSize(unsigned int width, unsigned int height);
  void setTitle(const std::string& title);

  void setActiveCamera(izz::SceneGraphEntity cameraEntity);

  // callbacks
  void onWindowResize(int width, int height);

 private:
  std::shared_ptr<izz::SceneGraphHelper> m_sceneGraph;
  std::shared_ptr<lsw::ResourceManager> m_resourceManager;
  std::shared_ptr<gui::GuiSystem> m_guiSystem{nullptr};
  std::shared_ptr<lsw::anim::AnimationSystem> m_animationSystem;
  entt::registry& m_registry;

  std::shared_ptr<gl::RenderSystem> m_renderSystem;
  std::shared_ptr<lsw::ecs::TransformSystem> m_transformSystem;
  std::shared_ptr<lsw::ecs::CameraSystem> m_cameraSystem;
  std::shared_ptr<lsw::ecs::DebugSystem> m_debugSystem;
  std::shared_ptr<lsw::io::InputSystem> m_inputSystem;
  std::shared_ptr<lsw::ecs::FirstPersonMovementSystem> m_firstPersonSystem;

  std::shared_ptr<WindowInputListener> m_genericInputListener;

  DisplayDetails m_displayDetails;
  std::string m_title = "Izzy Renderer";
};

}  // namespace gui
}  // namespace izz
