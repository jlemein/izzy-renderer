//
// Created by jeffrey on 12-12-21.
//
#include <gui_lighteditor.h>
#include <gui_mainmenu.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "izz_izzy.h"
using namespace izz::gui;

MainMenu::MainMenu(Izzy& izzy)
  : m_sceneGraph{izzy.sceneGraph}
  , m_materialEditor(izzy.materialSystem, izzy.sceneGraph) {}

void MainMenu::init() {
  m_materialEditor.init();
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
    if (ImGui::MenuItem("Material Editor..", "Ctrl+M")) {
      spdlog::info("Opening material editor");
      m_materialEditor.open();
    }
    if (ImGui::MenuItem("Light Control..", "Ctrl+L")) {
      spdlog::info("Opening light  editor");
      LightEditor::Show = true;
    }

    ImGui::EndMenu();
  }
  ImGui::EndMainMenuBar();

  m_materialEditor.render(time, dt);

}