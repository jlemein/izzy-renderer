//
// Created by jeffrey on 12-12-21.
//
#include <IconFontCppHeaders/IconsFontAwesome6.h>
#include <imgui.h>
#include <izz_relationshiputil.h>
#include <izzgl_entityfactory.h>
#include <izzgl_material.h>
#include <izzgl_materialsystem.h>
#include <izzgui_imguix.h>
#include <izzgui_materialinspector.h>

using namespace izz::gui;

namespace {
const ImColor BG_COLOR[2] = {ImColor(0x40, 0x40, 0x80, 128), ImColor(0x00, 0x00, 0x00, 128)};
const ImColor FG_COLOR = ImColor(255, 255, 255);
}  // namespace

izz::MaterialId MaterialInspector::SelectedMaterial = MATERIAL_UNDEFINED;

namespace {
/**
 * Converts a name to a readable name.
 * It strips of the part before the first dot and starts the name with a capital.
 * @param name Untransformed name.
 * @return The readable name for GUI purposes.
 */
std::string makeReadableName(std::string name) {
  auto dotIndex = name.find_first_of(".");
  if (dotIndex != std::string::npos) {
    name = name.substr(dotIndex + 1);
  }
  if (!name.empty()) {
    name[0] = toupper(name[0]);
  }

  return name;
}
}  // namespace

static bool p_open{false};

static int activeTab = 0;
static std::array<std::string, 2> tabs{"Materials", "Shaders"};

MaterialInspector::MaterialInspector(std::shared_ptr<gl::MaterialSystem> materialSystem)
  : m_materialSystem{materialSystem} {}

void MaterialInspector::init() {}

void MaterialInspector::render(float time, float dt) {
  izz::gl::Material mat;
  m_guiId = 0;

  if (MaterialInspector::Show) {
    if (ImGui::Begin(MaterialInspector::ID, &MaterialInspector::Show)) {
      //      if (ImGui::BeginChild("Child", ImVec2(0, 100), true)) {
      //        static char buf[256];
      //        ImGui::InputText("Path", buf, 256);
      //        if (ImGui::Button("Save")) {
      //          std::cout << "Save clicked";
      //        }
      //        ImGui::SameLine();
      //        if (ImGui::Button("Load")) {
      //          std::cout << "Load clicked";
      //        }
      //        ImGui::EndChild();
      //      }

      ImGui::PushStyleColor(ImGuiCol_Header, m_frameHeaderColor);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, m_inputBackgroundColor);

      if (SelectedMaterial != MATERIAL_UNDEFINED) {
        generalInfo();        // Shows the name, instance count and more general info.
        uniformProperties();  // properties that can be configured on the fly.
        compileConstants();   // compile constants that require recompilation.
      } else {
        ImGui::TextWrapped("Please select a material in the material navigator.");
      }
      ImGui::PopStyleColor(2);
    }
    ImGui::End();
  }
}

void MaterialInspector::generalInfo() {
  ImGui::PushID("UniformProperties");
  auto& material = m_materialSystem->getMaterialById(SelectedMaterial);

  if (ImGui::BeginTable("General Info", 2)) {
    // name
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Name:");
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(material.name.c_str());

    // instance count
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Instance Count:");
    ImGui::TableSetColumnIndex(1);
    auto& info = m_materialSystem->getShaderVariantInfo(material);
    ImGui::Text("%d", info.numberOfInstances);

    ImGui::EndTable();
  }

  ImGui::Separator();
  ImGui::PopID();
}

void MaterialInspector::uniformProperties() {
  ImGui::PushID("UniformProperties");
  auto& material = m_materialSystem->getMaterialById(SelectedMaterial);

  //  if (ImGui::CollapsingHeader("Blending")) {
  const char* items[] = {"Opaque", "Alpha blend", "Alpha Cutout"};
  static const char* currentItem = items[static_cast<int>(material.blendMode)];
  if (ImGui::BeginCombo("Blend mode", currentItem)) {
    for (int i = 0; i < 3; ++i) {
      bool isSelected = (currentItem == items[i]);
      if (ImGui::Selectable(items[i], isSelected)) {
        currentItem = items[i];
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  //  }

  if (ImGui::CollapsingHeader("Textures")) {
    for (auto& [file, texture] : material.textures) {
      ImGui::Text("%s  %s", ICON_FA_CHESS_BOARD, file.c_str());
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Image((void*)(intptr_t)texture.textureId, ImVec2(600, 600));
        ImGui::EndTooltip();
      }
    }
  }

  if (ImGui::CollapsingHeader("Uniform properties")) {
    for (auto& [name, value] : material.scopedUniforms->booleanValues) {
      ImGui::PushID(std::to_string(m_guiId++).c_str());

      auto readableName = makeReadableName(name);

      ImGui::Checkbox(readableName.c_str(), reinterpret_cast<bool*>(value->m_data));
      ImGui::PopID();
    }

    for (auto& [name, value] : material.scopedUniforms->intValues) {
      ImGui::PushID(std::to_string(m_guiId++).c_str());

      auto readableName = makeReadableName(name);

      ImGui::DragInt(readableName.c_str(), reinterpret_cast<int*>(value->m_data));
      ImGui::PopID();
    }

    for (auto& [name, value] : material.scopedUniforms->floatValues) {
      ImGui::PushID(std::to_string(m_guiId++).c_str());

      auto readableName = makeReadableName(name);

      ImGui::SliderFloat(readableName.c_str(), reinterpret_cast<float*>(value->m_data), 0.1F, 256.0F);
      ImGui::PopID();
    }

    for (auto& [name, value] : material.scopedUniforms->floatArrayValues) {
      ImGui::PushID(std::to_string(m_guiId++).c_str());

      auto readableName = makeReadableName(name);

      switch (value->m_dataType) {
        case PropertyType::FLOAT:
          ImGui::SliderFloat(readableName.c_str(), reinterpret_cast<float*>(value->m_data), 0.1F, 256.0F);
          break;
        case PropertyType::FLOAT2:
          ImGui::DragFloat2(readableName.c_str(), reinterpret_cast<float*>(value->m_data));
          break;

        case PropertyType::FLOAT3:
          ImGui::ColorEdit3(readableName.c_str(), reinterpret_cast<float*>(value->m_data));
          break;

        case PropertyType::FLOAT4:
          ImGui::ColorEdit4(readableName.c_str(), reinterpret_cast<float*>(value->m_data));
          break;

        default:
          ImGui::Text("Array value: to be added in future");
          break;
      }
      ImGui::PopID();
    }

    for (auto& [name, value] : material.scopedUniforms->intArrayValues) {
      ImGui::PushID(std::to_string(m_guiId++).c_str());

      auto readableName = makeReadableName(name);

      switch (value->m_dataType) {
        case PropertyType::INT:
          ImGui::DragInt(readableName.c_str(), reinterpret_cast<int*>(value->m_data));
          break;
        case PropertyType::INT2:
          ImGui::DragInt2(readableName.c_str(), reinterpret_cast<int*>(value->m_data));
          break;

        case PropertyType::INT3:
          ImGui::DragInt3(readableName.c_str(), reinterpret_cast<int*>(value->m_data));
          break;

        case PropertyType::INT4:
          ImGui::DragInt4(readableName.c_str(), reinterpret_cast<int*>(value->m_data));
          break;

        default:
          ImGui::Text("Array value: to be added in future");
          break;
      }
      ImGui::PopID();
    }
  }

  ImGui::PopID();
}

void MaterialInspector::compileConstants() {
  ImGui::PushID("CompileConstants");

  if (ImGui::CollapsingHeader("Compile constants")) {
    auto& matTemplate = m_materialSystem->getMaterialTemplateFromMaterial(SelectedMaterial);
    int numberOfCompileConstants = matTemplate.compileConstants.flags.size() + matTemplate.compileConstants.settings.size();
    if (numberOfCompileConstants <= 0) {
      ImGui::TextWrapped("No compile constants defined for this shader");
    }
    for (auto& name : matTemplate.compileConstants.flags) {
      ImGui::TextUnformatted(name.c_str());
    }
    ImGui::Separator();
    for (auto& [name, value] : matTemplate.compileConstants.settings) {
      ImGui::Text("%s: \t%s", name.c_str(), value.c_str());
    }
    ImGui::Dummy(ImVec2(0, 20));

    ImGui::TextWrapped("Changing compile constants require material recompilation");
    if (ImGui::Button("Recompile")) {
      spdlog::info("Recompiling shader");
    }
  }
  ImGui::PopID();
}

void MaterialInspector::Open() {
  MaterialInspector::Show = true;
}

void MaterialInspector::Toggle() {
  MaterialInspector::Show = !MaterialInspector::Show;
}