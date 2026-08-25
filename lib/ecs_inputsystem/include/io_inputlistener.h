//
// Created by jlemein on 29-11-20.
//

#pragma once

namespace izz {
namespace io {
class InputSystem;

class InputListener {
 public:
  virtual void init() = 0;
  virtual void update(const InputSystem* inputSystem) = 0;
};

}  // namespace io
}  // namespace izz
