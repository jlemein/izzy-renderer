//
// Created by jlemein on 29-11-20.
//

#ifndef GLVIEWER_IO_INPUTLISTENER_H
#define GLVIEWER_IO_INPUTLISTENER_H

namespace lsw
{
namespace io
{
class InputSystem;

class InputListener
{
 public:
  virtual void init() = 0;
  virtual void update(const InputSystem *inputSystem) = 0;
};

}
}

#endif  // GLVIEWER_IO_INPUTLISTENER_H
