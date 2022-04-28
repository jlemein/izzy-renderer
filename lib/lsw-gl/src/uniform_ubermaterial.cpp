//
// Created by jlemein on 07-03-21.
//
#include <uniform_ubermaterial.h>
#include <izzgl_material.h>
using namespace izz::ufm;
using namespace izz::gl;

void* UberUniformManager::CreateUniformBlock(size_t& t) {
  t = sizeof(Uber);
  return new Uber;
}

void UberUniformManager::DestroyUniformBlock(void* block) {
  auto uber = reinterpret_cast<Uber*>(block);
  delete uber;
}

void UberUniformManager::UpdateUniform(void* data, const Material& material) {
  auto uber = reinterpret_cast<Uber*>(data);

  const auto& ambientData = material.getUniformFloatArray("ambient_color");
  const auto& specularData = material.getUniformFloatArray("specular_color");
  const auto& diffuseData = material.getUniformFloatArray("diffuse_color");

  uber->ambient = glm::vec4(ambientData[0], ambientData[1], ambientData[2], 0.0F);
  uber->specular = glm::vec4(specularData[0], specularData[1], specularData[2], 0.0F);
  uber->diffuse = glm::vec4(diffuseData[0], diffuseData[1], diffuseData[2], 0.0F);
  uber->hasDiffuseTex = material.getTextureBuffer("diffuse_texture") >= 0;
}
