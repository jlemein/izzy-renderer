//
// Created by jlemein on 01-12-20.
//

#ifndef ARTIFAX_MESH_PRIMITIVEFACTORY_H
#define ARTIFAX_MESH_PRIMITIVEFACTORY_H

namespace lsw {
namespace geo {

struct Curve;
struct Mesh;

/**!
 * @brief
 * Factory for creating primitive types. Primitive types are elementary and
 * useful 3d shapes that are created based on the defined properties. These
 * shapes are therefore not loaded from a scene file format (such as *.fbx).
 *
 * @details
 * All primitive types are created with xz plane taken as the ground plane.
 * Width goes to positive X-axis, height to positive Y-axis and depth to
 * negative Z-axis.
 * The center of the shapes is always at the origin. This means everything else
 * except shapes on the flat XZ plane, need to be elevated up by half the height
 * if you want them to be aligned on the ground plane y = 0.
 */
class PrimitiveFactory {
public:
  /**
   * Creates a plane made of two triangles with the specified width and height.
   * @param width The width of the plane.
   * @param height The height of the plane.
   * @returns a plane wt
   */
  static Mesh MakePlane(float width, float height);

  /**!
   * Creates a simple axis aligned 3D box with center at origin, e.g. (0,0,0).
   * @param width Width along the X axis.
   * @param height Height along the Y axis.
   * @param depth Depth along the Z axis.
   * @return A mesh object.
   */
  static Mesh MakeBox(float width = 1.0F, float height = 1.0F, float depth = 1.0F);

  /**!
   * Creates a simple cylinder centered at origin, e.g. (0,0,0).
   * @param radius The radius of the circular cylinder cap.
   * @param height The height of the cylinder in the Y axis.
   * @param numSegments The number of sides for the cylinder. Increasing the
   * number of segments make a smoother result at the cost of more vertex data.
   * @returns a cylinder object
   */
  static Mesh MakeCylinder(float radius = 1.0F, float height = 1.0F, int numSides = 12);

  /**!
   * Creates a pyramid with a defined base and height.
   * @param base Base size of the pyramid, which corresponds to the width and height.
   * @param height The height of the pyramid from the base to the tip of the pyramid.
   * @return a mesh object
   */
  static Mesh MakePyramid(float base = 1.0F, float height = 1.0F);


  static Mesh MakeUVSphere(float radius = 1.0F, int num_segments = 32);
  static Mesh MakeDonut(float innerRadius, float outerRadius);
  static Mesh MakeTeapot(float size);
};

} // end of package
} // end of enterprise

#endif // GLVIEWER_MESH_PRIMITIVEFACTORY_H
