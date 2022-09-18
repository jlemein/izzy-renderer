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

static int activeTab = 0;
static std::array<std::string, 2> tabs{"Materials", "Shaders"};

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

  //  ImGui::Begin("TRYOUT");
  //  ImGui::BeginChild("Hello", ImVec2(100, 100), false, ImGuiWindowFlags_NoMove);
  //  ImGui::Button("Hello");
  //  ImGui::SetWindowPos(ImVec2(0, 768 - ImGui::GetWindowHeight() - 40), true);
  //  ImGui::EndChild();
  //  ImGui::End();

  if (m_show) {
    if (ImGui::Begin("Material Editor", &m_show)) {


      // -- tabs ---

      for (int i=0; i<tabs.size(); ++i) {
        if (i==activeTab) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2F, 0.389F, 0.619F, 1));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.137F, 0.267F, 0.424F, 1));
        }

        if (ImGui::Button(tabs[i].c_str(), ImVec2(150, 25))) {
          activeTab = i;
        }

        ImGui::SameLine(0, 2);
        ImGui::PopStyleColor(1);
      }
      ImGui::NewLine();

      if (activeTab == 0) {
        drawMaterialTable();
      }
      if (activeTab == 1) {
        drawShaderProgramTable();
      }
    }

    ImGui::End();  // "Material Editor"
  }

  m_shaderEditor.render(time, dt);
}

void MaterialEditor::drawMaterialTable() {
  constexpr int NUM_COLUMNS = 3;
  constexpr const char* columns[] = {
      "Material ID", "Material",  "Actions",
  };
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
      ImGui::PushID(static_cast<int>(id));
      ImGui::Text(material.name.c_str());

      ImGui::TableNextColumn();
      if (ImGui::Button("Edit")) {
        // m_shaderEditor.openDialog(e);
        spdlog::info("Open material editor for program id -- material id: {}", material.id);
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}


void MaterialEditor::drawShaderProgramTable() {
  constexpr int NUM_COLUMNS = 7;
  constexpr const char* columns[] = {
      "Program ID", "Name", "Vertex Shader", "Geometry Shader", "Fragment Shader", "Instance count", "Actions",
  };
  auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
  if (ImGui::BeginTable("ShaderProgramTable", NUM_COLUMNS, flags, ImVec2(0, 0), 0)) {
    // columns of the table
    for (auto name : columns) {
      ImGui::TableSetupColumn(name);
    }
    ImGui::TableHeadersRow();

    auto shaderPrograms = m_materialSystem->getShaderPrograms();
    if (shaderPrograms.empty()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("No programs to display");
    }

    for (const auto& [id, shader] : shaderPrograms) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(std::to_string(id).c_str());

      ImGui::TableNextColumn();
      ImGui::Text(shader.materialTemplateName.c_str());

      auto materialTemplate = m_materialSystem->getMaterialTemplate(shader.materialTemplateName);
      ImGui::TableNextColumn();
      ImGui::Text(materialTemplate.vertexShader.c_str());
      ImGui::TableNextColumn();
      ImGui::Text(materialTemplate.geometryShader.c_str());
      ImGui::TableNextColumn();
      ImGui::Text(materialTemplate.fragmentShader.c_str());


      ImGui::TableNextColumn();
      ImGui::PushID(static_cast<int>(id));
      ImGui::Text(std::to_string(shader.numberOfInstances).c_str());

      ImGui::TableNextColumn();
      if (ImGui::Button("Edit")) {
        // m_shaderEditor.openDialog(e);
        spdlog::info("Open shader editor for program id: {}", id);
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}

void MaterialEditor::open() {
  m_show = true;
}