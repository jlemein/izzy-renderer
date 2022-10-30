//
// Created by jeffrey on 11-11-22.
//
#pragma once

namespace izz {
namespace gui {

/**
 * A layout is used to perform initialization logic for the GUI system.
 * For example it can be used to initialize dockspace layouts for Imgui.
 */
class Layout {
 public:
  virtual ~Layout() = default;

  /// @brief Performs initialization code before the gui system calls all windows. Can be used to initialize a layout.
  virtual void onBeginFrame() = 0;
};

}  // namespace gui
}  // namespace izz