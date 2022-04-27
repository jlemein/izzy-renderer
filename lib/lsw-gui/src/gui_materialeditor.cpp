//
// Created by jeffrey on 12-12-21.
//
#include <gui_materialeditor.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include "ecs_name.h"
#include "gl_deferredrenderer.h"
#include "gl_renderable.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_material.h"
using namespace izz::gui;

static bool p_open{false};

MaterialEditor::MaterialEditor(std::shared_ptr<izz::SceneGraphHelper> sceneGraph)
  : m_sceneGraph{sceneGraph}
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

  if (m_show) {
    if (ImGui::Begin("Material Editor", &m_show)) {
      auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
      if (ImGui::BeginTable("Table1", 2, flags, ImVec2(0, 0), 0)) {
        ImGui::TableSetupColumn("Program ID");
        ImGui::TableSetupColumn("Material");
        //        ImGui::TableSetupColumn("Actions");
        ImGui::TableHeadersRow();

        for (const auto& [e, material] : m_sceneGraph->getRegistry().view<izz::gl::Material>().each()) {
          auto name = m_sceneGraph->getRegistry().get<lsw::ecs::Name>(e).name;
          auto r = m_sceneGraph->getRegistry().try_get<izz::gl::DeferredRenderable>(e);

          ImGui::TableNextColumn();
          ImGui::Text(r != nullptr ? std::to_string(static_cast<int>(r->materialId)).c_str() : "N.A.");
          ImGui::TableNextColumn();

          ImGui::PushID(static_cast<int>(e));
          ImGui::Text(fmt::format("{} ({})", name, material.name).c_str());
          ImGui::SameLine();
          if (ImGui::Button("Edit")) {
            m_shaderEditor.openDialog(e);
            spdlog::info("Open material editor for program id -- material id: {}", r->materialId);
          }
          ImGui::PopID();
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }

  m_shaderEditor.render(time, dt);
}

void MaterialEditor::open() {
  m_show = true;
}