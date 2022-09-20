//
// Created by jlemein on 07-02-21.
//

#pragma once

namespace izz {
namespace geo {

//class Light {
//public:
//  Light::Type type;
//  std::string name;
//  glm::vec3 diffuseColor;
//  glm::vec3 specularColor;
//  glm::vec3 ambientColor;
//
//  void* LightProperties;
//};


/**!
 * Represents a simple non-physics based description of a light source,
 * used mostly for simple rendering scenarios.
 */
class Light {
public:
  enum class Type { POINT_LIGHT, DIRECTIONAL_LIGHT, AMBIENT_LIGHT, SPOT_LIGHT, AREA_LIGHT };


  glm::vec3 getDiffuseColor() const;
  glm::vec3 getSpecularColor() const;
  glm::vec3 getAmbientColor() const;

  glm::vec3 getPosition() const;
  glm::vec3 getDirection() const;

  /**!
   * @returns the quadratic attenuation coefficient.
   */
  float getAttentuation();

  bool isDirectional() const;
  void setDirectional(bool isDirectional);

public:
  Light::Type type;
  std::string name;
  glm::vec3 position;
  glm::vec3 diffuseColor;
  glm::vec3 specularColor;
  glm::vec3 ambientColor;
  float attenuationLinear {0.0F};
  float attenuationQuadratic {1.0F};
  float intensity = 1.0F;
  glm::vec2 radius{1.0F};

  glm::vec3 upVector { 0.0F, 1.0F, 0.0F };
};

} // namespace geo
} // namespace lsw
