//
// Created by jeffrey on 25-9-22.
//
#pragma once

namespace izz {
namespace gl {

class SharedUniformBlock {
 public:
  /**
   * @brief Allows a one time preparation to collect data to prepare the data structure.
   *
   */
  void prepareData();

  /**
   * Pushes the prepared
   */
  void pushData();
};

}  // namespace gl
}  // namespace izz