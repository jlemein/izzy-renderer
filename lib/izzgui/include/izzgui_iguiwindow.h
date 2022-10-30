//
// Created by jlemein on 08-02-21.
//
#pragma once

namespace izz {
namespace gui {

class IGuiWindow {
 public:
  virtual void init() = 0;
  virtual void render(float time, float dt) = 0;
};

}  // namespace gui
}  // namespace izz
