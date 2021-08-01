//
// Created by jlemein on 07-03-21.
//
#include <geo_ubermaterialdata.h>
#include <geo_material.h>
using namespace lsw::geo;

UberMaterialData UberMaterialData::FromMaterial(const geo::Material &material) {
  UberMaterialData data;
  const auto& ambientData = material.getFloatArray("ambient_color");
  const auto& specularData = material.getFloatArray("specular_color");
  const auto& diffuseData = material.getFloatArray("diffuse_color");
  data.ambient = glm::vec4(ambientData[0], ambientData[1], ambientData[2], 0.0F);
  data.specular = glm::vec4(specularData[0], specularData[1], specularData[2], 0.0F);
  data.diffuse = glm::vec4(diffuseData[0], diffuseData[1], diffuseData[2], 0.0F);
  data.hasDiffuseTex = !material.getTexture("diffuse_texture").empty();

  return data;
}