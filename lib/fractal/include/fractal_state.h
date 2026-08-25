//
// Created by jlemein on 14-11-20.
//

#ifndef GLVIEWER_FRACTAL_STATE_H
#define GLVIEWER_FRACTAL_STATE_H

#include <array>
#include <vector>
#include <stack>

namespace izz {
namespace fractal {

struct State {
  std::stack<std::array<float, 3>> mPositionStack;
  std::stack<float> mRotationStack;

  std::vector<float> positions;
  std::array<float, 3> currentPosition{0.0F, 0.0F, 0.0F};
  float radians{0.0F};
};

} // end of package
} // end of enterprise name

#endif // GLVIEWER_FRACTAL_STATE_H
