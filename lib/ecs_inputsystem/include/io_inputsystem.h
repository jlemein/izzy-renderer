//
// Created by jlemein on 17-11-20.
//

#ifndef GLVIEWER_IO_INPUTSYSTEM_H
#define GLVIEWER_IO_INPUTSYSTEM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <list>

namespace affx
{
namespace io
{
class Command;
enum class Key;
class InputListener;

class InputSystem
{
 public:
  using KeyboardState = bool[26];
  InputSystem(GLFWwindow *window, int screenWidth, int screenHeight);

  void init();
  void update();

  bool isKeyPressed(char c) const;
  bool isKeyPressed(Key key) const;
  void getRelativeMouseMovement(double* dx, double* dy) const;

  void registerInputListener(std::shared_ptr<InputListener> listener);
  void unregisterInputListener(std::shared_ptr<InputListener> listener);

 private:
  GLFWwindow *m_window;
  int m_windowWidth, m_windowHeight;

  double m_currentMouseX {0.0}, m_currentMouseY {0.0};
  double m_prevMouseX {0.0}, m_prevMouseY {0.0};
  double m_relativeMouseX {0.0}, m_relativeMouseY {0.0};

  Command *m_buttonW;
  Command *m_buttonS;
  Command *m_buttonA;
  Command *m_buttonD;

  std::list<std::shared_ptr<InputListener>> m_inputListeners;
};

}  // namespace io
}  // namespace affx

#endif  // GLVIEWER_IO_INPUTSYSTEM_H
