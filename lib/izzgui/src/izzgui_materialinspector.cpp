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

void MaterialInspector::init() {
}

void MaterialInspector::render(float time, float dt) {
  izz::gl::Material mat;
  m_guiId = 0;

  if (MaterialInspector::Show) {
    if (ImGui::Begin(MaterialInspector::ID, &MaterialInspector::Show)) {
      if (ImGui::BeginChild("Child", ImVec2(0, 100), true)) {
        static char buf[256];
        ImGui::InputText("Path", buf, 256);
        if (ImGui::Button("Save")) {
          std::cout << "Save clicked";
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
          std::cout << "Load clicked";
        }
        ImGui::EndChild();
      }

      if (ImGui::BeginChild("Child2", ImVec2(0, 0), true)) {
        for (auto& [id, material] : m_materialSystem->getCreatedMaterials()) {
          if (ImGui::CollapsingHeader(material.name.c_str())) {
            ImGui::Text("Properties");
            ImGui::Separator();
            ImGui::Indent(m_indent);

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

            ImGui::Unindent(m_indent);
            ImGui::Text("Textures");
            ImGui::Separator();
            ImGui::Indent(m_indent);

            for (auto& [file, texture] : material.textures) {
              ImGui::Text("%s  %s", ICON_FA_CHESS_BOARD, file.c_str());
              if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Image((void*)(intptr_t)texture.textureId, ImVec2(600, 600));
                ImGui::EndTooltip();
              }
            }

            ImGui::Unindent(m_indent);
            ImGui::Text("Dynamic properties");
            ImGui::Separator();
            ImGui::Indent(m_indent);

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

            ImGui::Unindent(m_indent);
          }
        }

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::EndChild();
      }
    }
    ImGui::End();
  }
}

void MaterialInspector::Open() {
  MaterialInspector::Show = true;
}

void MaterialInspector::Toggle() {
  MaterialInspector::Show = !MaterialInspector::Show;
}