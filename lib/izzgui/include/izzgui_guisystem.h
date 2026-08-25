//
// Created by jlemein on 08-02-21.
//
#pragma once

#include <izzgui_iguiwindow.h>
#include <izzgui_iwindowextension.h>

#include <imgui.h>
#include <vector>
#include "izzgui_docklayout.h"
#include "izzgui_layout.h"

namespace izz {
class FontSystem;
namespace gui {

class Window;
struct DisplayDetails;

class GuiSystem : public IWindowExtension {
 public:
  GuiSystem() = default;
  GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem);
  GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem, std::shared_ptr<IGuiWindow> dialog);
  GuiSystem(std::shared_ptr<izz::FontSystem> fontSystem, std::vector<std::shared_ptr<IGuiWindow>> dialogs);

  void addDialog(std::shared_ptr<IGuiWindow> dialog);

  void setLayout(std::unique_ptr<izz::gui::Layout> layout);

  void initialize(izz::gui::Window* viewer) override;
  void update(float time, float dt) override;
  void beforeRender() override;
  void afterRender() override;
  void cleanup() override;

  bool isProcessingInput() const;

 private:
  std::shared_ptr<izz::FontSystem> m_fontSystem;
  std::vector<std::shared_ptr<IGuiWindow>> m_dialogs{};
  std::unique_ptr<Layout> m_layout {std::make_unique<izz::gui::DockLayout>()};

  void* m_windowHandle{nullptr};
  std::string m_shadingLanguage{""};
  std::string m_shadingVersion{""};

  ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::vector<ImFont*> m_fonts;
};

}  // namespace gui
}  // namespace izz