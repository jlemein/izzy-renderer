//
// Created by jlemein on 01-12-20.
//

#ifndef GLVIEWER_SHP_BOUNDINGBOX_H
#define GLVIEWER_SHP_BOUNDINGBOX_H

namespace artifax
{
namespace shp
{
struct BoundingBox {
  float xMin{.0F}, yMin{.0F}, zMin{.0F};
  float xMax{.0F}, yMax{.0F}, zMax{.0F};
};

}
}
#endif  // GLVIEWER_SHP_BOUNDINGBOX_H
