//
// Created by jlemein on 08-02-21.
//
#include <gui_system.h>
#include <vwr_viewer.h>
#include <imgui.h>
#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
using namespace lsw;

GuiSystem::GuiSystem(std::shared_ptr<lsw::viewer::Viewer> viewer, std::shared_ptr<IGuiWindow> window)
  : m_window(window)
  , m_viewer(viewer) {}

void GuiSystem::initialize() {
  m_windowHandle = m_viewer->getDisplayDetails().window;
  m_shadingLanguage = m_viewer->getDisplayDetails().shadingLanguage;
  if (m_shadingLanguage == "glsl") {
    m_shadingVersion = m_viewer->getDisplayDetails().shadingLanguageVersion;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  m_window->init();

  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(m_windowHandle), true);
  ImGui_ImplOpenGL3_Init(m_shadingVersion.c_str());
}
void GuiSystem::update(float time, float dt) {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  m_window->render(time, dt);
}

void GuiSystem::beforeRender() {
  // Rendering
  ImGui::Render();
}

void GuiSystem::afterRender() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiSystem::cleanup() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
