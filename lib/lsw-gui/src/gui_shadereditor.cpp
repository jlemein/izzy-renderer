//
// Created by jeffrey on 12-12-21.
//
#include <gui_shadereditor.h>
#include <fmt/format.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <ecsg_scenegraph.h>
#include "geo_material.h"
using namespace izz::gui;

ShaderEditor::ShaderEditor(std::shared_ptr<lsw::ecsg::SceneGraph> sceneGraph)
  : m_sceneGraph{sceneGraph} {}

void izz::gui::ShaderEditor::init() {}

void izz::gui::ShaderEditor::render(float time, float dt) {
  for (auto& [entity, show] : m_openDialogs) {
    if (show) {
      lsw::geo::Material material = m_sceneGraph->getRegistry().get<lsw::geo::Material>(entity);

      auto id = fmt::format("ShaderEditor_{}", static_cast<int>(entity));
      ImGui::PushID(id.c_str());
      ImGui::Begin("Shader Editor", &show);
        for (auto& p : material.unscopedUniforms.floatValues) {
          ImGui::DragFloat(p.first.c_str(), &p.second);
        }

        for (auto& p : material.userProperties.floatValues) {
          ImGui::DragFloat(p.first.c_str(), &p.second);
        }

        for (auto& p : material.userProperties.floatArrayValues) {
          ImGui::DragFloat4(p.first.c_str(), p.second.data(), 0.1F);
        }
      ImGui::End();
      ImGui::PopID();
    }
  }
}

void ShaderEditor::openDialog(entt::entity entity) {
  spdlog::info("Open shader editor for entity {}", static_cast<int>(entity));
  m_openDialogs[entity] = true;
}
