//
// Created by jlemein on 29-11-20.
//
#include <vwr_windowinputlistener.h>
#include <io_inputsystem.h>
#include <io_keyconstants.h>
using namespace lsw;
using namespace lsw::viewer;

WindowInputListener::WindowInputListener(GLFWwindow *window)
: m_window(window)
{
}
void WindowInputListener::init() {

}

void WindowInputListener::update(const io::InputSystem *inputSystem) {
  if (inputSystem->isKeyPressed(io::Key::kESCAPE)) {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
  }
}