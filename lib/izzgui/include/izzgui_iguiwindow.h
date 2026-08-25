//
// Created by jlemein on 08-02-21.
//
#pragma once

namespace izz {
namespace gui {

/**
 * Supports rendering behavior before the GUI is rendered.
 * This can be needed in case you need to render to a texture, but dont want to interfere with ImGui rendering.
 */
class IGuiRenderOffscreen {
  virtual void beforeGuiRender() = 0;
};

class IGuiWindow {
 public:
  virtual void init() = 0;
  virtual void render(float time, float dt) = 0;
};

}  // namespace gui
}  // namespace izz
