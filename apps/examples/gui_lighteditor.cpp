#include <gui_lighteditor.h>
#include <ecsg_scenegraph.h>
#include <imgui.h>
#include <ecs_light.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ecs_name.h>
#include <geo_material.h>
#include <georm_resourcemanager.h>
#include <ecs_transform.h>
using namespace lsw::gui;
using namespace lsw;
using namespace std;

GuiLightEditor::GuiLightEditor(shared_ptr<ecsg::SceneGraph> sceneGraph, shared_ptr<georm::FontSystem> fontSystem)
  : m_sceneGraph{sceneGraph}
  , m_fontSystem(fontSystem)
  , m_registry(sceneGraph->getRegistry()) {}

void GuiLightEditor::init() {
  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsDark();

  ImGuiIO& io = ImGui::GetIO();

  for (auto font : m_fontSystem->getAvailableFonts()) {
    m_fonts.push_back(io.Fonts->AddFontFromFileTTF(font.c_str(), 20));
    spdlog::info("Loaded font: {}", font);
  }
  spdlog::debug("Gui window initialized");
}

namespace {

void RenderDirectionalLightWindow(ecs::DirectionalLight& light, const char* name, ecs::Transform& transform, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Directional Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F, 0.0F);
    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::InputFloat3("Direction", glm::value_ptr(transform.localTransform[3]));
    ImGui::End();
  }
}

void RenderPointLightWindow(ecs::PointLight& light, const char* name, ecs::Transform& transform, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Point Light)", name).c_str(), &showWindow.at(name));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F);
    ImGui::DragFloat("Radius", &light.radius, 0.01F, 0.0F);
    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::InputFloat3("Position", glm::value_ptr(transform.localTransform[3]));
//    ImGui::DragFloat("Attenuation", &light.radius, 0.01F, 0.0F);
    ImGui::End();
  }
}

void RenderAmbientLightWindow(ecs::AmbientLight& light, const char* name, bool click) {
  static std::unordered_map<std::string, bool> showWindow;

  if (click || showWindow[name]) {
    showWindow[name] = true;

    ImGui::Begin(fmt::format("{} (Ambient Light)", name).c_str(), &showWindow.at(name));
    ImGui::ColorPicker3("Color", glm::value_ptr(light.color));
    ImGui::DragFloat("Intensity", &light.intensity, 0.01F);
    ImGui::End();
  }
}
}  // namespace

void GuiLightEditor::render(float dt, float totalTime) {
  // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
  ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);
  //  ImGui::StyleColorsLight();

  ImGui::PushFont(m_fonts[1]);

  // render your GUI
  ImGui::Begin("Demo window");
  ImGui::Button("Hello!");

  // == LIGHTS ========================================================
  ImGui::TextColored(ImVec4(1, 1, 0, 1), "Lights in the scene");

  auto dirLights = m_registry.view<ecs::DirectionalLight, ecs::Name, ecs::Transform>();
  for (auto [e, light, name, transform] : dirLights.each()) {
    RenderDirectionalLightWindow(light, name.name.c_str(), transform, ImGui::Button(name.name.c_str()));
  }

  auto ambientLights = m_registry.view<ecs::AmbientLight, ecs::Name>();
  for (auto [e, light, name] : ambientLights.each()) {
    RenderAmbientLightWindow(light, name.name.c_str(), ImGui::Button(name.name.c_str()));
  }
  auto pointLights = m_registry.view<ecs::PointLight, ecs::Name, ecs::Transform>();
  for (auto [e, light, name, transform] : pointLights.each()) {
    RenderPointLightWindow(light, name.name.c_str(), transform, ImGui::Button(name.name.c_str()));
  }
  // ==================================================================

  ImGui::TextColored(ImVec4(1, 1, 0, 1), "Materials");
  auto materials = m_registry.view<geo::Material>();

  ImGui::BeginChild("Scrolling");
  for (auto e : materials) {
    auto& m = materials.get<geo::Material>(e);
    ImGui::TextColored(ImVec4(0, 1, 1, 1), "%s", m.name.c_str());
    for (auto [name, path] : m.texturePaths) {
      ImGui::LabelText(name.c_str(), "%s", path.c_str());
    }
  }
  ImGui::EndChild();

  ImGui::End();
  ImGui::PopFont();
}