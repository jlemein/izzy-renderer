//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_FRACTAL_DRAWCOMMANDS_H
#define GLVIEWER_FRACTAL_DRAWCOMMANDS_H

#include <math.h>

namespace affx {
namespace fractal {

#include <fractal_tree.h>


class DrawForward {
  float mLength{1.0F};

public:
  DrawForward(float length = 1.0F) : mLength{length} {}

  void operator()(State &s) {
    s.positions.push_back(s.currentPosition[0]);
    s.positions.push_back(s.currentPosition[1]);
    s.positions.push_back(s.currentPosition[2]);

    float currentRotation = s.radians;
    s.currentPosition[0] += mLength * std::sin(currentRotation);
    s.currentPosition[1] += mLength * std::cos(currentRotation);

    s.positions.push_back(s.currentPosition[0]);
    s.positions.push_back(s.currentPosition[1]);
    s.positions.push_back(s.currentPosition[2]);
  }
};

class Rotate {
  float m_radians {0.0F};

public:
  Rotate(float radians) : m_radians (radians) {}

  void operator()(State& state) {
    state.radians += m_radians;
  }
};

class PushState {
public:
  PushState() {}

  void operator()(State &state) {
    state.mPositionStack.push(state.currentPosition);
    state.mRotationStack.push(state.radians);
  }
};

class PopState {
public:
  PopState() {}

  void operator()(State &state) {
    state.radians = state.mRotationStack.top();
    state.currentPosition = state.mPositionStack.top();

    state.mPositionStack.pop();
    state.mRotationStack.pop();
  }
};

class PushRotate {
  float mRadians{0.0F};

public:
  PushRotate(float radians) : mRadians(radians) {}

  void operator()(State &state) {
    state.mPositionStack.push(state.currentPosition);
    state.mRotationStack.push(state.radians);

    state.radians += mRadians;
  }
};

class PopRotate {
  float mRadians{0.0F};

public:
  PopRotate(float radians) : mRadians(radians) {}

  void operator()(State &state) {
    // pop back the position
    state.radians = state.mRotationStack.top();
    state.currentPosition = state.mPositionStack.top();

    state.mPositionStack.pop();
    state.mRotationStack.pop();

    // update rotation
    state.radians += mRadians;
  }
};

void draw_line(State &state) {
  float scale = 0.1F;
  state.currentPosition[0] += scale * std::sin(state.radians);
  state.currentPosition[1] += scale * std::cos(state.radians);
  //  state.currentPosition[2] += scale * std::sin(state.radians);
  state.positions.push_back(state.currentPosition[0]);
  state.positions.push_back(state.currentPosition[1]);
  state.positions.push_back(0.0F);
}

void rotate(State &state) { state.radians += 3.1415F * 0.125; }

}
}

#endif // GLVIEWER_FRACTAL_DRAWCOMMANDS_H
