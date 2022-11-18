//
// Created by jeffrey on 11-11-22.
//
#include <imgui.h>
#include <imgui_internal.h>
#include <izzgui_docklayout.h>

#include <izzgui_componenteditor.h>
#include <izzgui_materialeditor.h>
#include <izzgui_materialinspector.h>
#include <izzgui_scenenavigator.h>
#include <izzgui_materialnavigator.h>
using namespace izz::gui;

DockLayout::DockLayout(float left, float top, float right, float bottom)
  : m_left{left}
  , m_top{top}
  , m_right{right}
  , m_bottom{bottom} {}

void DockLayout::onBeginFrame() {
  // this line is called every frame
  auto dockspaceId = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

  // the following if-clause is called only once.
  static bool dockspaceCreated = false;
  if (!dockspaceCreated) {
    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);  // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

    ImGuiID dockMain = dockspaceId;
    ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, m_top, NULL, &dockMain);
    ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, m_bottom, NULL, &dockMain);
    ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2, NULL, &dockMain);
    ImGuiID dockLeft1 = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Up, 0.4, NULL, &dockLeft);
    ImGuiID dockLeft2 = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.4, NULL, &dockLeft);
    ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, m_right, NULL, &dockMain);
    ImGuiID dockRight1 = ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Up, 0.4, NULL, &dockRight);
    ImGuiID dockRight2 = ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.4, NULL, &dockRight);

    ImGui::DockBuilderDockWindow(SceneNavigator::ID, dockLeft1);
    ImGui::DockBuilderDockWindow(ComponentEditor::ID, dockLeft2);
    ImGui::DockBuilderDockWindow(MaterialEditor::ID, dockBottom);
    ImGui::DockBuilderDockWindow(MaterialNavigator::ID, dockRight1);
    ImGui::DockBuilderDockWindow(MaterialInspector::ID, dockRight2);

    ImGui::DockBuilderFinish(dockspaceId);
    dockspaceCreated = true;
  }
}
