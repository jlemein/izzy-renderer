//
// Created by jeffrey on 11-11-22.
//
#pragma once

#include <izzgui_layout.h>

namespace izz {
namespace gui {

class DockLayout : public Layout {
 public:
  DockLayout() = default;
  DockLayout(float left, float top, float right, float bottom);

  virtual ~DockLayout() = default;

  void onBeginFrame() override;

 private:
  float m_left {0.2};
  float m_top {0.2};
  float m_right{0.2};
  float m_bottom{0.2};
};
}
}
