//
// Created by jeffrey on 12-12-21.
//
#include <gui_materialeditor.h>
#include <ecsg_scenegraph.h>
#include <entt/entt.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <geo_material.h>
#include "glrs_renderable.h"
#include "ecs_name.h"
using namespace izz::gui;

static bool p_open {false};

MaterialEditor::MaterialEditor(std::shared_ptr<lsw::ecsg::SceneGraph> sceneGraph)
  : m_sceneGraph{sceneGraph} {}

void MaterialEditor::init() {}

void MaterialEditor::render(float time, float dt) {

  if (m_show) {
    if (ImGui::Begin("Material Editor", &m_show)) {
      auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
      if (ImGui::BeginTable("Table1", 2, flags, ImVec2(0, 0), 0)) {

        ImGui::TableSetupColumn("Program ID");
        ImGui::TableSetupColumn("Material");
        ImGui::TableHeadersRow();

        for (const auto& [e, material] : m_sceneGraph->getRegistry().view<lsw::geo::Material>().each()) {
          auto name = m_sceneGraph->getRegistry().get<lsw::ecs::Name>(e).name;
          auto r = m_sceneGraph->getRegistry().try_get<lsw::glrs::Renderable>(e);

          ImGui::TableNextColumn();
          ImGui::Text(r != nullptr ? std::to_string(static_cast<int>(r->program)).c_str() : "N.A.");
          ImGui::TableNextColumn();
          ImGui::Text(fmt::format("{} ({})", name, material.name).c_str());
          ImGui::TableNextRow();
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }

}

void MaterialEditor::open() {
  m_show = true;
}