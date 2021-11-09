//
// Created by jlemein on 08-02-21.
//

#ifndef RENDERER_GUI_SYSTEM_H
#define RENDERER_GUI_SYSTEM_H

#include <vwr_viewerextension.h>
#include <imgui.h>

namespace lsw {
namespace viewer {
class Viewer;
struct DisplayDetails;
}
}

class IGuiWindow {
 public:
  virtual void init() = 0 ;
  virtual void render(float time, float dt) = 0;
};

class GuiSystem : public lsw::ecs::IViewerExtension {
public:
  GuiSystem(std::shared_ptr<IGuiWindow> dialog);
  GuiSystem(std::vector<std::shared_ptr<IGuiWindow>> dialogs);

  void initialize(lsw::viewer::Viewer* viewer) override;
  void update(float time, float dt) override;
  void beforeRender() override;
  void afterRender() override;
  void cleanup() override;

  bool isProcessingInput() const;

private:
//  std::shared_ptr<IGuiWindow> m_window {nullptr};
  std::vector<std::shared_ptr<IGuiWindow>> m_dialogs {};

  void* m_windowHandle {nullptr};
  std::string m_shadingLanguage {""};
  std::string m_shadingVersion {""};

  ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool show_demo_window = true;
  bool show_another_window = false;
};

#endif // RENDERER_GUI_SYSTEM_H
