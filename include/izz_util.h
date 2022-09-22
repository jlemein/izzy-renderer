//
// Created by jlemein on 21-09-22.
//

#pragma once

#include <math.h>
namespace izz {
struct Util {
  static inline double RadiansToDegrees(double radians) {
    return 180.0 * radians / M_PI;
  }
  static inline float RadiansToDegrees(float radians) {
    return 180.0F * radians / static_cast<float>(M_PI);
  }

  static inline double DegreesToRadians(double degrees) {
    return M_PI * degrees / 180.0;
  };
  static inline float DegreesToRadians(float degrees) {
    return static_cast<float>(M_PI) * degrees / 180.0F;
  };
};

}  // namespace izz

