//
// Created by jlemein on 06-11-20.
//

#ifndef INCLUDED_VIEWER_MESH_H_
#define INCLUDED_VIEWER_MESH_H_

#include <vector>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <iostream>

#include <shp_boundingbox.h>

namespace artifax {
namespace shp {

struct Mesh {
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<uint32_t> indices;

  BoundingBox getBoundingBox() {
    BoundingBox bbox;
    for (int i=0; i<vertices.size(); i+=3) {
      bbox.xMin = vertices[i] < bbox.xMin ? vertices[i] : bbox.xMin;
      bbox.yMin = vertices[i+1] < bbox.yMin ? vertices[i] : bbox.yMin;
      bbox.zMin = vertices[i+2] < bbox.zMin ? vertices[i] : bbox.zMin;

      bbox.xMax = vertices[i] > bbox.xMax ? vertices[i] : bbox.xMax;
      bbox.yMax = vertices[i+1] > bbox.yMax ? vertices[i] : bbox.yMax;
      bbox.zMax = vertices[i+2] > bbox.zMax ? vertices[i] : bbox.zMax;
    }
    return bbox;
  }
};

} // end package
} // end enterprise

#endif // INCLUDED_VIEWER_MESH_H_
