//
// Created by jlemein on 07-03-21.
//
#include <uniform_ubermaterial.h>
#include <geo_material.h>
using namespace lsw::ufm;
using namespace lsw::geo;

void* UberUniformManager::CreateUniformBlock(size_t& t) {
  t = sizeof(Uber);
  return new Uber;
}

void UberUniformManager::DestroyUniformBlock(void* block) {
  auto uber = reinterpret_cast<Uber*>(block);
  delete uber;
}

void UberUniformManager::UpdateUniform(void* data, Material& material) {
  auto uber = reinterpret_cast<Uber*>(data);

  const auto& ambientData = material.userProperties.getFloatArray("ambient_color");
  const auto& specularData = material.userProperties.getFloatArray("specular_color");
  const auto& diffuseData = material.userProperties.getFloatArray("diffuse_color");

  uber->ambient = glm::vec4(ambientData[0], ambientData[1], ambientData[2], 0.0F);
  uber->specular = glm::vec4(specularData[0], specularData[1], specularData[2], 0.0F);
  uber->diffuse = glm::vec4(diffuseData[0], diffuseData[1], diffuseData[2], 0.0F);
  uber->hasDiffuseTex = !material.getTexture("diffuse_texture").empty();
}
