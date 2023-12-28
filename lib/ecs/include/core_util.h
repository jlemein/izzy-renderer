//
// Created by jlemein on 09-08-21.
//

#ifndef RENDERER_CORE_UTIL_H
#define RENDERER_CORE_UTIL_H

#include <math.h>
namespace lsw {
namespace core {
struct Util {
  static inline double ToDegrees(double radians) { return 180.0 * radians / M_PI; }
  static inline float ToDegrees(float radians) { return 180.0F * radians / static_cast<float>(M_PI); }

  static inline double ToRadians(double degrees){return M_PI * degrees / 180.0; };
  static inline float ToRadians(float degrees){return static_cast<float>(M_PI) * degrees / 180.0F; };
};

}
}

#endif // RENDERER_CORE_UTIL_H
