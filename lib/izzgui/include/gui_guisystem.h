//
// Created by jlemein on 08-02-21.
//
#pragma once

#include <gui_iguiwindow.h>
#include <gui_iwindowextension.h>

#include <imgui.h>
#include <vector>

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

  void initialize(izz::gui::Window* viewer) override;
  void update(float time, float dt) override;
  void beforeRender() override;
  void afterRender() override;
  void cleanup() override;

  bool isProcessingInput() const;

 private:
  //  std::shared_ptr<IGuiWindow> m_window {nullptr};
  std::shared_ptr<izz::FontSystem> m_fontSystem;
  std::vector<std::shared_ptr<IGuiWindow>> m_dialogs{};

  void* m_windowHandle{nullptr};
  std::string m_shadingLanguage{""};
  std::string m_shadingVersion{""};

  ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool show_demo_window = true;
  bool show_another_window = false;
  std::vector<ImFont*> m_fonts;
};

}  // namespace gui
}  // namespace izz