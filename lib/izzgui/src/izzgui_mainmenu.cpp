//
// Created by jeffrey on 12-12-21.
//
#include <imgui.h>
#include <izzgui_mainmenu.h>
#include <spdlog/spdlog.h>
#include <izz_izzy.h>
#include <izzgui_stats.h>
using namespace izz::gui;

MainMenu::MainMenu(Izzy& izzy)
  : m_sceneGraph{izzy.entityFactory}
  {}

void MainMenu::init() {
}

void MainMenu::render(float time, float dt) {
  ImGui::BeginMainMenuBar();
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
    }
    if (ImGui::MenuItem("Close", "Ctrl+W")) { /* */
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Edit")) {
    if (ImGui::MenuItem(ComponentEditor::ID, "Ctrl+Alt+C")) {
      spdlog::info("Opening component editor");
      ComponentEditor::Open();
    }
    if (ImGui::MenuItem(SceneNavigator::ID, "Ctrl+Alt+N")) {
      spdlog::info("Opening scene navigator");
      SceneNavigator::Open();
    }
    if (ImGui::MenuItem(MaterialInspector::ID, "Ctrl+Alt+M")) {
      spdlog::info("Opening material inspector");
      MaterialInspector::Open();
    }
    if (ImGui::MenuItem(MaterialNavigator::ID, "Ctrl+Alt+J")) {
      spdlog::info("Opening material navigator");
      MaterialInspector::Open();
    }
    if (ImGui::MenuItem(MaterialEditor::ID, "Ctrl+Alt+E")) {
      spdlog::info("Opening material editor");
      MaterialEditor::Open();
    }

    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("View")) {
    if (ImGui::MenuItem("Toggle Statistics..", "K", &izz::gui::StatsDialog::Show)) { /* Do stuff */
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Tools")) {
    if (ImGui::MenuItem("Cube map editor", "Ctrl+Shift+C", &izz::gui::CubeMapTool::Show)) { /* Do stuff */
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();
}