//
// Created by jlemein on 17-11-20.
//

#ifndef GLVIEWER_IO_COMMAND_H
#define GLVIEWER_IO_COMMAND_H

namespace affx {
namespace io {

class Command {
public:
  virtual void execute() = 0;
};

} // end of package
} // end of enterprise name

#endif // GLVIEWER_IO_COMMAND_H
