//
// Created by jeffrey on 12-12-21.
//
#include <fmt/format.h>
#include <gui_shadereditor.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "izz_entityfactory.h"
#include "izzgl_material.h"
using namespace izz::gui;

ShaderEditor::ShaderEditor(std::shared_ptr<izz::EntityFactory> sceneGraph)
  : m_sceneGraph{sceneGraph} {}

void izz::gui::ShaderEditor::init() {}

void izz::gui::ShaderEditor::render(float time, float dt) {
  for (auto& [entity, show] : m_openDialogs) {
    if (show) {
      auto& material = m_sceneGraph->getRegistry().get<izz::gl::Material>(entity);

      auto id = fmt::format("ShaderEditor_{}", static_cast<int>(entity));
      ImGui::PushID(id.c_str());
//      ImGui::Begin("Shader Editor", &show);
//        for (auto& p : material.unscopedUniforms.floatValues) {
//          ImGui::DragFloat(p.first.c_str(), &p.second);
//          material.setUniformFloat(p.first.c_str(), 0.1);
//          p.second = 100.0;
//        }
//
//        for (auto& p : material.userProperties.floatValues) {
//          ImGui::DragFloat(p.first.c_str(), &p.second);
//          material.setUniformFloat(p.first.c_str(), 0.1);
//          p.second = 100.0;
//        }
//
//        for (auto& p : material.userProperties.floatArrayValues) {
//          ImGui::DragFloat4(p.first.c_str(), p.second.data(), 0.1F);
//        }
//      ImGui::End();
      ImGui::PopID();
    }
  }
}

void ShaderEditor::openDialog(entt::entity entity) {
  spdlog::info("Open shader editor for entity {}", static_cast<int>(entity));
  m_openDialogs[entity] = true;
}
