//
// Created by jlemein on 29-11-20.
//
#include <gui_windowinputlistener.h>
#include <io_inputsystem.h>
#include <io_keyconstants.h>
using namespace izz;
using namespace izz::gui;

WindowInputListener::WindowInputListener(GLFWwindow *window)
: m_window(window)
{
}
void WindowInputListener::init() {

}

void WindowInputListener::update(const lsw::io::InputSystem *inputSystem) {
  if (inputSystem->isKeyPressed(lsw::io::Key::kESCAPE)) {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
  }
}