//
// Created by jlemein on 17-11-20.
//

#pragma once

namespace izz {
namespace io {

class Command {
 public:
  virtual void execute() = 0;
};

}  // namespace io
}  // namespace izz
