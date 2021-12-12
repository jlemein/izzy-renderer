#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_transform.h>
#include <ecsg_scenegraph.h>
#include <geo_material.h>
#include <gui_lighteditor.h>
#include <izz_resourcemanager.h>
#include <izz_texturesystem.h>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace izz::gui;
using namespace lsw;
using namespace std;

LightEditor::LightEditor(shared_ptr<ecsg::SceneGraph> sceneGraph, shared_ptr<lsw::FontSystem> fontSystem)
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

void LightEditor::render(float dt, float totalTime) {
  if (LightEditor::Show) {
    // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);
    //  ImGui::StyleColorsLight();

    if (!m_fonts.empty()) {
      ImGui::PushFont(*m_fonts.begin());
    }

    // render your GUI
    ImGui::Begin("Light Control", &LightEditor::Show);

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

    ImGui::End();

    if (!m_fonts.empty()) {
      ImGui::PopFont();
    }
  }
}

ResourceInspector::ResourceInspector(std::shared_ptr<ResourceManager> resourceManager)
  : m_resourceManager{resourceManager} {}

void ResourceInspector::init() {
  //    m_resourceManager->getTextures();
}

void ResourceInspector::render(float time, float dt) {
  // render your GUI
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("Resource Inspector");
  ImGui::Button("Hello!");

  //    ImGui::Begin("OpenGL Texture Text");
  //    auto texResource = m_resourceManager->getTextures().begin()->second;
  //    auto& tex = **texResource;

  //    ImGui::EndChild();
  if (ImGui::BeginTable("Textures", 3)) {
    //        for (int row = 0; row < 4; row++)
    //        {
    ImGui::TableNextRow();
    for (const auto& [name, tex] : m_resourceManager->getTextureSystem()->getTextures()) {
      ImGui::TableSetColumnIndex(0);
      //            spdlog::warn(name);
      ImGui::Text(name.c_str());
    }
    //            for (int column = 0; column < 3; column++)
    //            {
    //                ImGui::TableSetColumnIndex(column);
    //                ImGui::Text("Row %d Column %d", row, column);
    //            }
    //        }
    ImGui::EndTable();
  }

  ImGui::End();
}
