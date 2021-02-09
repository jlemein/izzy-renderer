//
// Created by jlemein on 20-01-21.
//

#ifndef RENDERER_GEO_TRANSFORM_H
#define RENDERER_GEO_TRANSFORM_H

namespace affx {
namespace geo {

template <typename T>
struct Transform {
  T matrix[4][4];
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_TRANSFORM_H
