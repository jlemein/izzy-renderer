//
// Created by jlemein on 08-02-21.
//

#ifndef RENDERER_GUI_SYSTEM_H
#define RENDERER_GUI_SYSTEM_H

#include <vwr_viewerextension.h>
#include <imgui.h>

namespace affx {
namespace viewer {
class Viewer;
struct DisplayDetails;
}
}

class GuiSystem : public affx::ecs::IViewerExtension {
public:
  GuiSystem(std::shared_ptr<affx::viewer::Viewer> viewer);

  void initialize() override;
  void update(float time, float dt) override;
  void beforeRender() override;
  void afterRender() override;
  void cleanup() override;

private:
  std::shared_ptr<affx::viewer::Viewer> m_viewer {nullptr};
  void* m_windowHandle {nullptr};
  std::string m_shadingLanguage {""};
  std::string m_shadingVersion {""};

  ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool show_demo_window = true;
  bool show_another_window = false;
};

#endif // RENDERER_GUI_SYSTEM_H
