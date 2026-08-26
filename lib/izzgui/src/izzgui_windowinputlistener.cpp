//
// Created by jlemein on 29-11-20.
//
#include <io_inputsystem.h>
#include <io_keyconstants.h>
#include <izzgui_componenteditor.h>
#include <izzgui_materialeditor.h>
#include <izzgui_materialinspector.h>
#include <izzgui_materialnavigator.h>
#include <izzgui_scenenavigator.h>
#include <izzgui_stats.h>
#include <izzgui_windowinputlistener.h>
using namespace izz;
using namespace izz::gui;

WindowInputListener::WindowInputListener(GLFWwindow* window)
  : m_window(window) {}
void WindowInputListener::init() {}

void WindowInputListener::update(const izz::io::InputSystem* inputSystem) {
  if (inputSystem->isKeyPressed(izz::io::Key::kESCAPE)) {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
  }

  if (inputSystem->isKeyPressed('K')) {
    // make possible to do toggle by changing input system.
    izz::gui::StatsDialog::Show = true;  // !izz::gui::FpsStats::Show;
  }

  if (inputSystem->isKeyPressed(io::Key::kLEFT_CONTROL) && inputSystem->isKeyPressed(io::Key::kLEFT_ALT)) {
    if (inputSystem->isKeyPressed('C')) {
      izz::gui::ComponentEditor::Open();
    }
    if (inputSystem->isKeyPressed('E')) {
      izz::gui::MaterialEditor::Open();
    }
    if (inputSystem->isKeyPressed('M')) {
      izz::gui::MaterialInspector::Open();
    }
    if (inputSystem->isKeyPressed('N')) {
      izz::gui::SceneNavigator::Open();
    }
    if (inputSystem->isKeyPressed('J')) {
      izz::gui::MaterialNavigator::Open();
    }
  }
}