//
// Created by jlemein on 07-03-21.
//

#ifndef RENDERER_GEO_UBERMATERIALDATA_H
#define RENDERER_GEO_UBERMATERIALDATA_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace lsw {
namespace geo {

struct Material;

//TODO: this is implementation specific for OpenGL, move it to separate namespace
struct UberMaterialData {
  glm::vec4 diffuse{1.0F, 0.0F, 0.0F, 0.0F};
  glm::vec4 specular;
  glm::vec4 ambient;
  bool hasDiffuseTex {false};

  static inline const char *PARAM_NAME = "UberMaterial";
  //TODO: should be factored out to util
  /**
   * Fills a UberMaterial data structure with the properties from the specified
   * material.
   * The UberMaterialData is used for UberMaterial materials.json.
   *
   * @param material
   * @return
   */
  static UberMaterialData FromMaterial(const geo::Material& material);
};


struct LambertData {
  static inline const char *PARAM_NAME = "Lambert";

  static LambertData FromMaterial(const geo::Material& material);
};


}
}

#endif // RENDERER_GEO_UBERMATERIALDATA_H
