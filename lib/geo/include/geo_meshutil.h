//
// Created by jlemein on 07-08-21.
//

#ifndef RENDERER_GEO_MESHUTIL_H
#define RENDERER_GEO_MESHUTIL_H

namespace lsw {
namespace geo {

class Mesh;

class MeshUtil {
public:
  static void GenerateBinormalTangents(geo::Mesh& m);
};

} // namespace geo
} // namespace lsw

#endif // RENDERER_GEO_MESHUTIL_H
