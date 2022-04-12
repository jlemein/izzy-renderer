//
// Created by jlemein on 01-12-20.
//
#pragma once

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
   * Creates a plane in the XZ plane made of two triangles with the specified width and depth.
   * @param width The width of the plane.
   * @param height The depth of the plane.
   * @returns a mesh object with coordinates, normals, tangents and uv coordinates defined.
   */
  static Mesh MakePlane(const std::string& name, float width, float depth);

  /**
   * Creates a plane in the XY plane made of two triangles with the specified width and height.
   * @param width The width of the plane.
   * @param height The height of the plane.
   * @returns a plane with coordinates, uv coordinates defined.
   */
  static Mesh MakePlaneXY(const std::string& name, float width, float height);

  /**!
   * Creates a simple axis aligned 3D box with center at origin, e.g. (0,0,0).
   * @param name Name of the mesh.
   * @param width Width along the X axis.
   * @param height Height along the Y axis.
   * @param depth Depth along the Z axis.
   * @return A mesh object.
   */
  static Mesh MakeBox(const std::string& name, float width = 1.0F, float height = 1.0F, float depth = 1.0F);

  /**!
   * Creates a simple cylinder centered at origin, e.g. (0,0,0).
   * @param name The name of the mesh.
   * @param radius The radius of the circular cylinder cap.
   * @param height The height of the cylinder in the Y axis.
   * @param numSegments The number of sides for the cylinder. Increasing the
   * number of segments make a smoother result at the cost of more vertex data.
   * @returns a cylinder object
   */
  static Mesh MakeCylinder(const std::string& name, float radius = 1.0F, float height = 1.0F, int numSides = 12);

  /**!
   * Creates a pyramid with a defined base and height.
   * @param name The name of the mesh.
   * @param base Base size of the pyramid, which corresponds to the width and height.
   * @param height The height of the pyramid from the base to the tip of the pyramid.
   * @return a mesh object
   */
  static Mesh MakePyramid(const std::string& name, float base = 1.0F, float height = 1.0F);


  /**
   *
   * @param name The name of the mesh.
   * @param radius
   * @param num_segments
   * @return
   */
  static Mesh MakeUVSphere(const std::string& name, float radius = 1.0F, int num_segments = 32);

  static Mesh MakeDonut(float innerRadius, float outerRadius);
  static Mesh MakeTeapot(float size);
};

} // end of package
} // end of enterprise
