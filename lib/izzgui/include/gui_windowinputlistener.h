//
// Created by jlemein on 29-11-20.
//
#pragma once

#include <io_inputlistener.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace izz {
namespace io {
class InputSystem;
}

namespace gui {

/**
 * Listens to generic key presses to close the window, and showing the GUI.
 */
class WindowInputListener : public lsw::io::InputListener {
 public:
  WindowInputListener(GLFWwindow* window);

  void init() override;

  void update(const lsw::io::InputSystem* inputSystem) override;

 private:
  GLFWwindow* m_window;
};

}  // namespace gui
}  // namespace izz
