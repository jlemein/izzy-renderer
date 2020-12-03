//
// Created by jlemein on 01-12-20.
//

#ifndef GLVIEWER_MESH_PRIMITIVEFACTORY_H
#define GLVIEWER_MESH_PRIMITIVEFACTORY_H

#include <shp_mesh.h>

namespace artifax
{
namespace shp
{
/**!
 * Collection of primitive types.
 * All primitive types are created with xz plane taken as the ground plane.
 * Width goes to positive X-axis, height to positive Y-axis and depth to negative Z-axis.
 */
class PrimitiveFactory
{
 public:
  static Mesh MakePlane(float width, float height);
  static Mesh MakeBox(float width, float height, float depth);
  static Mesh MakePyramid(float size, float height);
  static Mesh MakeSphere(float radius);
  static Mesh MakeDonut(float innerRadius, float outerRadius);
  static Mesh MakeTeapot(float size);
};

} // end package
} // end enterprise
#endif  // GLVIEWER_MESH_PRIMITIVEFACTORY_H
