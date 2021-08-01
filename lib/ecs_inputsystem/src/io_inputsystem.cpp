//
// Created by jlemein on 17-11-20.
//
#include <io_command.h>
#include <io_inputsystem.h>
#include <io_keyconstants.h>
#include <io_inputlistener.h>

#include <unordered_map>
using namespace lsw::io;

namespace {
std::unordered_map<Key, int> toGlfwKey = {
  {Key::kESCAPE, GLFW_KEY_ESCAPE},
  {Key::kSPACEBAR, GLFW_KEY_SPACE},
  {Key::kLEFT_SHIFT, GLFW_KEY_LEFT_SHIFT}
};
}

InputSystem::InputSystem(GLFWwindow* window, int screenWidth, int screenHeight)
    : m_window(window), m_windowWidth{screenWidth}, m_windowHeight{screenHeight} {
}

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

bool InputSystem::isKeyPressed(char key) const
{
  return glfwGetKey(m_window, GLFW_KEY_A + (key - 'A')) == GLFW_PRESS;
}

bool InputSystem::isKeyPressed(Key key) const
{
  return glfwGetKey(m_window, toGlfwKey.at(key)) == GLFW_PRESS;
}

void InputSystem::getRelativeMouseMovement(double *dx, double *dy) const
{
  *dx = m_relativeMouseX / m_windowWidth;
  *dy = m_relativeMouseY / m_windowHeight;
}

void InputSystem::registerInputListener(std::shared_ptr<InputListener> listener) {
  m_inputListeners.emplace_back(listener);
}

void InputSystem::unregisterInputListener(std::shared_ptr<InputListener> listener) {
  m_inputListeners.remove(listener);
}