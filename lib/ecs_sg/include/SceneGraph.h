//
// Created by jlemein on 12-01-21.
//

#ifndef GLVIEWER_SCENEGRAPH_H
#define GLVIEWER_SCENEGRAPH_H


/**!
 * Provides utility
 */
class SceneGraph {
public:
  SceneGraph();

  typedef Entity Scen;

  std::shared_ptr<Light> createLight();
  std::shared_ptr<Light> createAmbientLight();

  /**!
   * @brief Creates a standard entity. Every entity has the following components
   * assigned:
   * - Id: a global unique identifier (even over different scene graphs)
   * - Name: a distinguishable name for reading purposes
   * - Transform        A transformation
   * - Relationship
   * @return
   */
  std::shared_ptr<Light> createPhysicalEntity();

  /**!
   * It is also possible to create entities that do not correspond to materialistic
   * objects in the scene. Think about ambient light
   * @return
   */
  std::shared_ptr<Light> createEntity();




};
#endif // GLVIEWER_SCENEGRAPH_H
