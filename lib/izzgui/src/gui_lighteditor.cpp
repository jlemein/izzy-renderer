#include <ecs_light.h>
#include <ecs_transform.h>
#include <gui_lighteditor.h>
#include "../../izzgl/include/izz_resourcemanager.h"
#include "izzgl_entityfactory.h"
#include "izzgl_material.h"
#include "izzgl_texturesystem.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace izz::gui;
using namespace izz;
using namespace std;

LightEditor::LightEditor(shared_ptr<izz::EntityFactory> sceneGraph, shared_ptr<izz::FontSystem> fontSystem)
  : m_sceneGraph{sceneGraph}
  , m_fontSystem(fontSystem)
  , m_registry(sceneGraph->getRegistry()) {}

void LightEditor::init() {
  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsDark();

  ImGuiIO& io = ImGui::GetIO();

  for (auto font : m_fontSystem->getAvailableFonts()) {
    m_fonts.push_back(io.Fonts->AddFontFromFileTTF(font.name.c_str(), 20));
    spdlog::info("Loaded font: {}", font.name);
  }
  spdlog::debug("Gui window initialized");
}

namespace {

void RenderDirectionalLightWindow(ecs::DirectionalLight& light, const char* name, ecs::Transform& transform, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Directional Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F, 0.0F, 5.0F);
    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::DragFloat3("Direction", glm::value_ptr(transform.localTransform[3]), 0.01F, -1.0, 1.0);
    ImGui::End();
  }
}

void RenderPointLightWindow(ecs::PointLight& light, const char* name, ecs::Transform& transform, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Point Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F, 0.0F, 5.0F);
    ImGui::DragFloat("Radius", &light.radius, 0.01F, 0.0F);
    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::DragFloat3("Position", glm::value_ptr(transform.localTransform[3]), 0.01F);
    ImGui::End();
  }
}

void RenderSpotLightWindow(ecs::SpotLight& light, const char* name, ecs::Transform& transform, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Point Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, .01F, 0.0F, 100.0F);
    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::SliderAngle("Inner cone", &light.penumbra, .0F, 90.F);
    ImGui::SliderAngle("Outer cone", &light.umbra, .0F, 90.F);
    ImGui::DragFloat3("Position", glm::value_ptr(transform.localTransform[3]), 0.01F);
    ImGui::DragFloat3("Direction", glm::value_ptr(transform.localTransform[3]), 0.01F);
    ImGui::End();
  }
}


void RenderAmbientLightWindow(ecs::AmbientLight& light, const char* name, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Ambient Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F, 0.0F, 1.0F);
    ImGui::ColorPicker3("Color", glm::value_ptr(light.color));
    ImGui::End();
  }
}
}  // namespace

void LightEditor::render(float dt, float totalTime) {
  if (LightEditor::Show) {
    // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);

    if (!m_fonts.empty()) {
      ImGui::PushFont(*m_fonts.begin());
    }

    // render your GUI
    ImGui::Begin("Light Control", &LightEditor::Show);

    // == LIGHTS ========================================================
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Lights in the scene");

    auto dirLights = m_registry.view<ecs::DirectionalLight, izz::Name, ecs::Transform>();
    for (auto [e, light, name, transform] : dirLights.each()) {
      RenderDirectionalLightWindow(light, name.name.c_str(), transform, ImGui::Button(name.name.c_str()));
    }

    auto ambientLights = m_registry.view<ecs::AmbientLight, izz::Name>();
    for (auto [e, light, name] : ambientLights.each()) {
      RenderAmbientLightWindow(light, name.name.c_str(), ImGui::Button(name.name.c_str()));
    }
    auto pointLights = m_registry.view<ecs::PointLight, izz::Name, ecs::Transform>();
    for (auto [e, light, name, transform] : pointLights.each()) {
      RenderPointLightWindow(light, name.name.c_str(), transform, ImGui::Button(name.name.c_str()));
    }

    auto spotLights = m_registry.view<ecs::SpotLight, izz::Name, ecs::Transform>();
    for (auto [e, light, name, transform] : spotLights.each()) {
      RenderSpotLightWindow(light, name.name.c_str(), transform, ImGui::Button(name.name.c_str()));
    }
    // ==================================================================

    ImGui::End();

    if (!m_fonts.empty()) {
      ImGui::PopFont();
    }
  }
}
