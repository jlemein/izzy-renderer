//
// Created by jlemein on 21-03-21.
//

#ifndef RENDERER_ECSG_ENTITYFACTORY_H
#define RENDERER_ECSG_ENTITYFACTORY_H

namespace lsw {
namespace ecsg {

/**!
 * @brief Factory for creating entities.
 */
class EntityFactory {
public:
  EntityFactory
  SceneGraphEntity makeEntity();
  /// @brief Creates a simple barebone entity containing minimum components
  /// Minum components are: Transform, Name, Relationship
  SceneGraphEntity makeEntity(std::string name = "");

  SceneGraphEntity makeCamera(std::string name, float fovx = 120.0F,
                              float aspect = 1.0F, float zNear = 0.1F,
                              float zFar = 1000.0F);

  SceneGraphEntity makeCamera(const geo::Camera &geoCamera);

  SceneGraphEntity makeLight(const geo::Light &light);

  SceneGraphEntity makePointLight(glm::vec3 intensity = {1.0F, 1.0F, 1.0F},
                                  glm::vec3 color = {1.0F, 1.0F, 1.0F},
                                  std::string name = "#Light#");
  SceneGraphEntity makePointLight(std::string name, glm::vec3 position);
  SceneGraphEntity makeDirectionalLight(std::string name, glm::vec3 direction);

};

} // end of package
} // end of enterprise
#endif // RENDERER_ECSG_ENTITYFACTORY_H
