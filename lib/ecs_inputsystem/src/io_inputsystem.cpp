//
// Created by jlemein on 17-11-20.
//
#include <io_command.h>
#include <io_inputlistener.h>
#include <io_inputsystem.h>
#include <io_keyconstants.h>

#include <unordered_map>
using namespace izz::io;

namespace {
std::unordered_map<Key, int> toGlfwKey = {{Key::kESCAPE, GLFW_KEY_ESCAPE},         {Key::kSPACEBAR, GLFW_KEY_SPACE},
                                          {Key::kLEFT_SHIFT, GLFW_KEY_LEFT_SHIFT}, {Key::kRIGHT_SHIFT, GLFW_KEY_RIGHT_SHIFT},
                                          {Key::kLEFT_ALT, GLFW_KEY_LEFT_ALT},     {Key::kLEFT_CONTROL, GLFW_KEY_LEFT_CONTROL}};
}

InputSystem::InputSystem(GLFWwindow* window, int width, int height)
  : m_window(window)
  , m_windowWidth{width}
  , m_windowHeight{height} {}

void InputSystem::init() {
  // hide mouse cursor and reset to center screen
  //  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(m_window, &m_currentMouseX, &m_currentMouseY);

  for (auto& listener : m_inputListeners) {
    listener->init();
  }
}

void InputSystem::update() {
  m_prevMouseX = m_currentMouseX;
  m_prevMouseY = m_currentMouseY;

  glfwGetCursorPos(m_window, &m_currentMouseX, &m_currentMouseY);

  m_relativeMouseX = m_currentMouseX - m_prevMouseX;
  m_relativeMouseY = m_currentMouseY - m_prevMouseY;

  for (auto& listener : m_inputListeners) {
    listener->update(this);
  }
}

bool InputSystem::isKeyPressed(char key) const {
  return glfwGetKey(m_window, GLFW_KEY_A + (key - 'A')) == GLFW_PRESS;
}

bool InputSystem::isKeyPressed(Key key) const {
  int glfwKeyCode = toGlfwKey.at(key);
  return glfwGetKey(m_window, glfwKeyCode) == GLFW_PRESS;
}

bool InputSystem::isMouseButtonPressed(MouseButton button) const {
  auto glfwButton = GLFW_MOUSE_BUTTON_LEFT + static_cast<int>(button);
  return glfwGetMouseButton(m_window, glfwButton) == GLFW_PRESS;
}

void InputSystem::getRelativeMouseMovement(double* dx, double* dy) const {
  *dx = m_relativeMouseX / m_windowWidth;
  *dy = m_relativeMouseY / m_windowHeight;
}

void InputSystem::setWindowSize(int width, int height) {
  m_windowWidth = width;
  m_windowHeight = height;
}

void InputSystem::registerInputListener(std::shared_ptr<InputListener> listener) {
  m_inputListeners.emplace_back(listener);
}

void InputSystem::unregisterInputListener(std::shared_ptr<InputListener> listener) {
  m_inputListeners.remove(listener);
}