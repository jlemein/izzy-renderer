//
// Created by jlemein on 01-12-21.
//
#include <geo_materialutil.h>
#include <geo_material.h>
#include <iostream>

using namespace lsw;
using namespace lsw::geo;

geo::Material MaterialUtil::CloneMaterial(const geo::Material& material) {
  auto clonedMaterial = material;

  const auto& userUbo = material.properties.at(material.userProperties.ubo_name);
  clonedMaterial.properties[material.userProperties.ubo_name].data = new char[userUbo.size];
  memcpy(clonedMaterial.properties[material.userProperties.ubo_name].data, userUbo.data, userUbo.size);

  return clonedMaterial;
}