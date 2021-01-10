//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_MESH_PRIMITIVEFACTORY_H
#define ARTIFAX_MESH_PRIMITIVEFACTORY_H

namespace artifax {
namespace shp {

class Curve;
class Mesh;

/**!
 * Collection of primitive types.
 * All primitive types are created with xz plane taken as the ground plane.
 * Width goes to positive X-axis, height to positive Y-axis and depth to
 * negative Z-axis.
 */
class PrimitiveFactory {
public:
  static Mesh MakePlane(float width, float height);

  /**!
   * Creates a simple axis aligned 3D box with center at origin, e.g. (0,0,0).
   * @param width Width along the X axis.
   * @param height Height along the Y axis.
   * @param depth Depth along the Z axis.
   * @return A mesh object.
   */
  static Mesh MakeBox(float width, float height, float depth);

  /**!
   * Creates a simple cylinder centered at origin, e.g. (0,0,0).
   * @param radius The radius of the circular cylinder cap.
   * @param height The height of the cylinder in the Y axis.
   * @param numSegments The number of sides for the cylinder. Increasing the
   * number of segments make a smoother result at the cost of more vertex data.
   * @return
   */
  static Mesh MakeCylinder(float radius, float height, int numSides = 12);

//  static Mesh MakeBoxWireframe(float width, float height, float depth);
  /// returns a box modeled as a curve, representing a wireframe model

  static Mesh MakePyramid(float size, float height);
  static Mesh MakeSphere(float radius);
  static Mesh MakeDonut(float innerRadius, float outerRadius);
  static Mesh MakeTeapot(float size);
};

} // namespace shp
} // namespace artifax
#endif // GLVIEWER_MESH_PRIMITIVEFACTORY_H
