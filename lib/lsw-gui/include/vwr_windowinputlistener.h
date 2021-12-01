//
// Created by jlemein on 29-11-20.
//

#ifndef GLVIEWER_VIEWER_WINDOWINPUTLISTENER_H
#define GLVIEWER_VIEWER_WINDOWINPUTLISTENER_H

#include <io_inputlistener.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace lsw {
namespace io {
class InputSystem;
}

namespace viewer {

/**
 * Listens to generic key presses to close the window, and showing the GUI.
 */
class WindowInputListener : public io::InputListener {
 public:
  WindowInputListener(GLFWwindow* window);

  void init() override;

  void update(const io::InputSystem* inputSystem) override;

 private:
  GLFWwindow* m_window;
};

}  // namespace viewer
}  // namespace lsw
#endif  // GLVIEWER_VIEWER_WINDOWINPUTLISTENER_H
