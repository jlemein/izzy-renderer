//
// Created by jeffrey on 30-9-22.
//
#pragma once

#include <gui_iguiwindow.h>
#include <izz_statcounter.h>

namespace izz {
//class StatCounter;

namespace gui {

/**
 * Statistics window showing FPS, frame times and render system statistics.
 */
class StatsDialog : public IGuiWindow {
 public:
  StatsDialog();

  virtual void init() override;
  virtual void render(float time, float dt) override;

  static inline bool Show{false};

 private:
  StatCounter* m_statCounter{nullptr};
};

}  // namespace gui
}  // namespace izz
