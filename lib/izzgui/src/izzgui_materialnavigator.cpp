//
// Created by jeffrey on 12-12-21.
//
#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <ecs_camera.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <izz_relationshiputil.h>
#include <izzgl_entityfactory.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgui_imguix.h>
#include <izzgui_materialnavigator.h>
#include <spdlog/spdlog.h>
#include "izzgui_materialinspector.h"
using namespace izz::gui;

ImRect MaterialNavigator::renderHierarchy(izz::gl::Material& material, int depth) {
  auto icon = ICON_FA_CHESS_BOARD;
  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;

  if (m_selectedMaterial == material.id) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  std::string treeNodeText = fmt::format("{}  {}", icon, material.name);
  bool isClicked{false};
  ImGuiX::TreeNode(treeNodeText.c_str(), flags, depth, isClicked);

  if (isClicked) {
    m_selectedMaterial = material.id;
    izz::gui::MaterialInspector::SelectedMaterial = material.id;
  }
//  else {
////    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
//      m_selectedMaterial = MATERIAL_UNDEFINED;
//      izz::gui::MaterialInspector::SelectedMaterial = MATERIAL_UNDEFINED;
////    }
//  }

  ImRect nodeRect;
  return nodeRect;
}

MaterialNavigator::MaterialNavigator(std::shared_ptr<izz::EntityFactory> sceneGraph, std::shared_ptr<izz::gl::MaterialSystem> materialSystem)
  : m_sceneGraph{sceneGraph}
  , m_materialSystem{materialSystem} {}

void MaterialNavigator::init() {}

void MaterialNavigator::render(float time, float dt) {
  if (MaterialNavigator::Show) {
    if (ImGui::Begin(MaterialNavigator::ID, &MaterialNavigator::Show)) {
      for (auto& [id, material] : m_materialSystem->getCreatedMaterials()) {
        renderHierarchy(material);
      }
    }
    ImGui::End();
  }
}

void MaterialNavigator::Open() {
  MaterialNavigator::Show = true;
}

void MaterialNavigator::Toggle() {
  MaterialNavigator::Show = !MaterialNavigator::Show;
}