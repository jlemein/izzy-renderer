//
// Created by jeffrey on 12-12-21.
//
#include <gui_materialeditor.h>
#include <imgui.h>
#include <izzgl_materialsystem.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include "ecs_name.h"
#include "gl_deferredrenderer.h"
#include "gl_renderable.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_material.h"
using namespace izz::gui;

static bool p_open{false};

MaterialEditor::MaterialEditor(std::shared_ptr<gl::MaterialSystem> materialSystem, std::shared_ptr<izz::SceneGraphHelper> sceneGraph)
  : m_materialSystem{materialSystem}
  , m_sceneGraph{sceneGraph}
  , m_shaderEditor{sceneGraph} {}

void MaterialEditor::init() {
  m_shaderEditor.init();
}

void MaterialEditor::render(float time, float dt) {
  static bool showEditor = false;
  izz::gl::Material mat;

  ImGui::Begin("TRYOUT");
  ImGui::BeginChild("Hello", ImVec2(100, 100), false, ImGuiWindowFlags_NoMove);
  ImGui::Button("Hello");
  ImGui::SetWindowPos(ImVec2(0, 768 - ImGui::GetWindowHeight() - 40), true);
  ImGui::EndChild();
  ImGui::End();

  constexpr int NUM_COLUMNS = 4;
  constexpr const char* columns[] = {
    "Material ID",
    "GL program ID",
    "Material",
    "Actions"
  };

  if (m_show) {
    if (ImGui::Begin("Material Editor", &m_show)) {
      auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
      if (ImGui::BeginTable("Table1", NUM_COLUMNS, flags, ImVec2(0, 0), 0)) {
        // columns of the table
        for (auto name : columns) {
          ImGui::TableSetupColumn(name);
        }
        ImGui::TableHeadersRow();

        auto materials = m_materialSystem->getCreatedMaterials();
        if (materials.empty()) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("No materials to display");
        }

        for (const auto& [id, material] : materials) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text(std::to_string(id).c_str());

          ImGui::TableNextColumn();
          ImGui::Text(std::to_string(material.programId).c_str());

          ImGui::TableNextColumn();
          ImGui::PushID(static_cast<int>(id));
          ImGui::Text(fmt::format("{} ({})", material.name, material.name).c_str());

          ImGui::TableNextColumn();
//          ImGui::SameLine();
          if (ImGui::Button("Edit")) {
            //m_shaderEditor.openDialog(e);
            spdlog::info("Open material editor for program id -- material id: {}", material.id);
          }
          ImGui::PopID();
        }
        ImGui::EndTable();
      }

      ImGui::End();  // "Material Editor"
    }
  }

  m_shaderEditor.render(time, dt);
}

void MaterialEditor::open() {
  m_show = true;
}