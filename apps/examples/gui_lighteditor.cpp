#include <gui_lighteditor.h>
#include <ecsg_scenegraph.h>
#include <imgui.h>
#include <ecs_light.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ecs_name.h>
#include <geo_material.h>
using namespace lsw::gui;
using namespace std;

GuiLightEditor::GuiLightEditor(shared_ptr<ecsg::SceneGraph> sceneGraph)
  : m_sceneGraph{sceneGraph}
  , m_registry(sceneGraph->getRegistry()) {}

void GuiLightEditor::operator()(float dt, float totalTime) {
  // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
  ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);
//  ImGui::StyleColorsLight();

  // render your GUI
  ImGui::Begin("Demo window");
  ImGui::Button("Hello!");

  auto view = m_registry.view<ecs::DirectionalLight>();
  for (auto e : view) {
    auto& light = view.get<ecs::DirectionalLight>(e);
    auto& name = m_registry.get<ecs::Name>(e);

    ImGui::TextColored(ImVec4(1,1,0,1), "Lights in the scene");
    ImGui::ColorEdit3(name.name.c_str(), glm::value_ptr(light.color));
  }

  ImGui::TextColored(ImVec4(1,1,0,1), "Materials");
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
}