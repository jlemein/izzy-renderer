//
// Created by jeffrey on 11-11-22.
//
#include <ecs_transform.h>
#include <imgui.h>
#include <izz_izzy.h>
#include <izzgui_componenteditor.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "IconFontCppHeaders/IconsFontAwesome6.h"
#include "ecs_camera.h"
#include "ecs_firstpersoncontrol.h"
#include "izz_skybox.h"
using namespace izz::gui;

entt::entity ComponentEditor::SelectedEntity = entt::null;

ComponentEditor::ComponentEditor(izz::Izzy& izzy)
  : m_izzy{izzy} {}

void ComponentEditor::init() {}

void ComponentEditor::render(float time, float dt) {
  auto& reg = m_izzy.getRegistry();
  bool hasMenuBar = SelectedEntity != entt::null;

  ImGuiWindowFlags windowFlags = 0;
  if (hasMenuBar) {
    windowFlags |= ImGuiWindowFlags_MenuBar;
  }

  if (ComponentEditor::Show) {
    if (ImGui::Begin(ComponentEditor::ID, &ComponentEditor::Show, windowFlags)) {
      if (hasMenuBar) {
        addComponentMenuBar();
      }

      ImGui::PushStyleColor(ImGuiCol_Header, m_frameHeaderColor);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, m_inputBackgroundColor);
      if (SelectedEntity != entt::null) {
        nameComponent();
        transformComponent();
        cameraComponent();
        fpsControlComponent();

        // lights
        directionalLightComponent();
        pointLightComponent();
        spotLightComponent();
        ambientLightComponent();

        skyboxComponent();

      } else {
        ImGui::TextUnformatted("Please select an entity in the navigator panel.");
      }
      ImGui::PopStyleColor(2);
    }
    ImGui::End();
  }
}

void ComponentEditor::addComponentMenuBar() {
  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("Add")) {
    ImGui::MenuItem("Name");
    ImGui::MenuItem("Transform");
    ImGui::MenuItem("Relationship");
    ImGui::MenuItem("Camera");
    if (ImGui::BeginMenu("User input")) {
      ImGui::MenuItem("FPS Control");
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Lighting")) {
      ImGui::MenuItem("Ambient");
      ImGui::MenuItem("Directional");
      ImGui::MenuItem("Point");
      ImGui::MenuItem("Spot");
      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();
}

static int TextCallback(ImGuiInputTextCallbackData* data) {
  auto nameComponent = reinterpret_cast<izz::Name*>(data->UserData);
  nameComponent->name = data->Buf;
  return 0;
}

void ComponentEditor::nameComponent() {
  ImGui::PushID("NameComponent");
  if (auto nameComponent = m_izzy.getRegistry().try_get<Name>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Name##Header")) {
      // copy the std::string name into the null-terminated buffer for editing.
      // we don't want to directly manipulate the std::string data.
      // the callback makes sure the value is reflected in the component.
      static char BUFF[32] = "\0";
      int maxLength = std::min(static_cast<int>(nameComponent->name.size()) + 1, 32);
      std::strncpy(BUFF, nameComponent->name.c_str(), maxLength);
      BUFF[31] = '\0';

      ImGuiTextFlags flags = ImGuiInputTextFlags_None;
      flags |= ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackEdit;
      ImGui::InputText("Name", BUFF, 32, flags, &TextCallback, nameComponent);
    }
  }
  ImGui::PopID();
}

void ComponentEditor::transformComponent() {
  ImGui::PushID("TransformComponent");
  if (auto transform = m_izzy.getRegistry().try_get<ecs::Transform>(SelectedEntity); transform != nullptr) {
    if (ImGui::CollapsingHeader("Transform")) {
      ImGui::DragFloat3("Position", glm::value_ptr(transform->localTransform[3]), 0.01F, -1000.0F, 1000.0F);
    }
  }
  ImGui::PopID();
}

void ComponentEditor::cameraComponent() {
  ImGui::PushID("CameraComponent");
  if (auto camera = m_izzy.getRegistry().try_get<ecs::Camera>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Camera")) {
      ImGui::DragFloat2("Near / Far Plane", &camera->zNear, 0.1F, 0.1F, 100000.0F);
      ImGui::DragFloat("Aspect Ratio", &camera->aspect, 0.01F, 0.01F, 10.0F);
      ImGui::DragFloat("Field of view (X)", &camera->fovx, 0.1F, 0.1F, 2 * M_PI);
    }
  }
  ImGui::PopID();
}

void ComponentEditor::fpsControlComponent() {
  ImGui::PushID("FpsControlComponent");
  if (auto fps = m_izzy.getRegistry().try_get<ecs::FirstPersonControl>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("First Person Control")) {
      ImGui::DragFloat("H. Sensitivity", &fps->sensitivityX, 0.1F, 0.1F, 10.0F);
      ImGui::DragFloat("V. Sensitivity", &fps->sensitivityY, 0.1F, 0.1F, 10.0F);
      ImGui::Checkbox("Rotate on mouse press", &fps->onlyRotateOnMousePress);
    }
  }
  ImGui::PopID();
}

void ComponentEditor::skyboxComponent() {
  ImGui::PushID("SkyboxComponent");
  if (auto skybox = m_izzy.getRegistry().try_get<izz::Skybox>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Skybox")) {
      ImGui::Checkbox("Enabled", &skybox->isEnabled);

      static char buff[100];
      static bool showTemplates{true};
      static bool showMaterialInstances{true};
      izz::gl::Material& currentMaterial = m_izzy.materialSystem->getMaterialById(skybox->material);

      if (ImGui::BeginCombo("Material", currentMaterial.name.c_str())) {
        ImGui::InputTextWithHint("##Search", ICON_FA_MAGNIFYING_GLASS "Search materials...", buff, 100);
        ImGui::Checkbox("Templates", &showTemplates);
        ImGui::SameLine();
        ImGui::Checkbox("Instances", &showMaterialInstances);

        if (showMaterialInstances) {
          for (auto [id, material] : m_izzy.materialSystem->getCreatedMaterials()) {
            if (ImGui::Selectable(material.name.c_str(), skybox->material == id)) {
              skybox->material = id;
              spdlog::info("Selected material has id {}", skybox->material);
            }
          }
        }

        // only add seperator if both templates and instances are shown.
        if (showMaterialInstances && showTemplates) {
          ImGui::Separator();
        }

        if (showTemplates) {
          for (auto& [name, materialTemplate] : m_izzy.materialSystem->getMaterialTemplates()) {
            if (ImGui::Selectable(name.c_str(), false)) {
              spdlog::info("Creating material {}", name);
              skybox->material = m_izzy.materialSystem->createMaterial(name).id;
              spdlog::info("Created material {} has id {}", name, skybox->material);
            }
          }
        }
        ImGui::EndCombo();
      }
    }
  }
  ImGui::PopID();
}

void ComponentEditor::directionalLightComponent() {
  ImGui::PushID("DirectionalLightComponent");
  if (auto light = m_izzy.getRegistry().try_get<ecs::DirectionalLight>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Directional Light")) {
      ImGui::DragFloat("Intensity", &light->intensity, 0.01F, 0.0F, 5.0F);
      ImGui::ColorEdit3("Color", glm::value_ptr(light->color));
    }
  }
  ImGui::PopID();
}

void ComponentEditor::pointLightComponent() {
  ImGui::PushID("PointLightComponent");
  if (auto light = m_izzy.getRegistry().try_get<ecs::PointLight>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Point Light")) {
      ImGui::DragFloat("Intensity", &light->intensity, 0.01F, 0.0F, 5.0F);
      ImGui::DragFloat("Radius", &light->radius, 0.01F, 0.0F);
      ImGui::ColorEdit3("Color", glm::value_ptr(light->color));
    }
  }
  ImGui::PopID();
}

void ComponentEditor::spotLightComponent() {
  ImGui::PushID("SpotLightComponent");
  if (auto light = m_izzy.getRegistry().try_get<ecs::SpotLight>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Spot Light")) {
      ImGui::DragFloat("Intensity", &light->intensity, .01F, 0.0F, 100.0F);
      ImGui::ColorEdit3("Color", glm::value_ptr(light->color));
      ImGui::SliderAngle("Inner cone", &light->penumbra, .0F, 90.F);
      ImGui::SliderAngle("Outer cone", &light->umbra, .0F, 90.F);
    }
  }
  ImGui::PopID();
}
void ComponentEditor::ambientLightComponent() {
  ImGui::PushID("AmbientLightComponent");
  if (auto light = m_izzy.getRegistry().try_get<ecs::AmbientLight>(SelectedEntity)) {
    if (ImGui::CollapsingHeader("Ambient Light")) {
      ImGui::DragFloat("Intensity", &light->intensity, 0.01F, 0.0F, 1.0F);
      ImGui::ColorPicker3("Color", glm::value_ptr(light->color));
    }
  }
  ImGui::PopID();
}

void ComponentEditor::Open() {
  ComponentEditor::Show = true;
}

void ComponentEditor::Toggle() {
  ComponentEditor::Show = !ComponentEditor::Show;
}