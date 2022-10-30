//
// Created by jlemein on 08-02-21.
//
#include <izzgui_guisystem.h>
#include <izzgui_iguiwindow.h>
#include <izzgui_window.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <memory>

#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <izz_fontsystem.h>
#include <spdlog/spdlog.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace izz;
using namespace izz::gui;

GuiSystem::GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem)
  : m_fontSystem{fontSystem} {}

GuiSystem::GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem, std::shared_ptr<IGuiWindow> dialog)
  : m_fontSystem{fontSystem}
  , m_dialogs{dialog} {}

GuiSystem::GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem, std::vector<std::shared_ptr<IGuiWindow>> dialogs)
  : m_fontSystem{fontSystem}
  , m_dialogs{dialogs.begin(), dialogs.end()} {}

void GuiSystem::addDialog(std::shared_ptr<IGuiWindow> dialog) {
  m_dialogs.emplace_back(dialog);
}

void GuiSystem::initialize(Window* viewer) {
  m_windowHandle = viewer->getDisplayDetails().window;
  m_shadingLanguage = viewer->getDisplayDetails().shadingLanguage;
  if (m_shadingLanguage == "glsl") {
    m_shadingVersion = viewer->getDisplayDetails().shadingLanguageVersion;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  for (auto font : m_fontSystem->getAvailableFonts()) {
    m_fonts.push_back(io.Fonts->AddFontFromFileTTF(font.name.c_str(), font.preferredSize));
    spdlog::info("Loaded font: {}", font.name);
  }

  // load additional icons
  static const ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
  ImFontConfig iconsConfig;
  iconsConfig.MergeMode = true;
  iconsConfig.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF("fonts/" FONT_ICON_FILE_NAME_FAS, 16.0F, &iconsConfig, iconsRanges);

  for (const auto& dialog : m_dialogs) {
    dialog->init();
  }

  // enable docking
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(m_windowHandle), true);
  ImGui_ImplOpenGL3_Init(m_shadingVersion.c_str());
}

void GuiSystem::setLayout(std::unique_ptr<izz::gui::Layout> layout) {
  m_layout = std::move(layout);
}

void GuiSystem::update(float time, float dt) {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  m_layout->onBeginFrame();

  for (const auto& dialog : m_dialogs) {
    dialog->render(dt, time);
  }
}

void GuiSystem::beforeRender() {
  ImGui::Render();
}

void GuiSystem::afterRender() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiSystem::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

bool GuiSystem::isProcessingInput() const {
  return ImGui::IsAnyItemActive();
}
