//
// Created by jeffrey on 1-10-22.
//
#include <imgui.h>
#include <izz_statcounter.h>
#include <izzgui_stats.h>

using namespace izz::gui;

StatsDialog::StatsDialog()
  : m_statCounter{StatCounter::instance()} {}

void StatsDialog::init() {}

void StatsDialog::render(float time, float dt) {
#ifndef NDBEBUG
  if (StatsDialog::Show) {
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
    if (!ImGui::Begin("Frame rendering", &StatsDialog::Show,
                      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                          ImGuiWindowFlags_NoMouseInputs)) {
      ImGui::End();
      return;
    }

    int fromRight = std::min(180.F, ImGui::GetWindowContentRegionMax().x - 150);

    ImGui::Text("Frame rate:");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
    ImGui::Text("%.2f FPS", m_statCounter->getFps());

    ImGui::Text("Frame duration:");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
    ImGui::Text("%.3f ms", m_statCounter->getFrameDuration());

//    auto forward_duration = m_statCounter->getDuration("forward_render_frame");
//    auto deferred_duration = m_statCounter->getDuration("deferred_render_frame");
    auto totalFrameDuration = m_statCounter->getDuration("total_render_frame");
//    std::cout << "Duration in ms: " << totalFrameDuration << std::endl;

//    ImGui::Text("Forward:");
//    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
//    ImGui::Text("%.3f ms (%.2f FPS)", forward_duration, 1000.0/forward_duration);
//
//    ImGui::Text("Deferred:");
//    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
//    ImGui::Text("%.3f ms (%.2f FPS)", deferred_duration, 1000.0/deferred_duration);

    ImGui::Text("Real time:");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
    ImGui::Text("%.3f ms (%.2f FPS)", totalFrameDuration, 1000.0/totalFrameDuration);

    ImGui::Text("Program swaps:");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
    ImGui::Text("%d per frame", m_statCounter->getCountPerFrame("useProgram"));

    ImGui::Text("Opaque / Transp:");
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - fromRight);
    ImGui::Text("%d / %d", m_statCounter->getCountPerFrame("OPAQUE_OBJECTS"),
                m_statCounter->getCountPerFrame("TRANSPARENT_OBJECTS"));

    ImGui::End();
  }
#endif  // NDEBUG
}