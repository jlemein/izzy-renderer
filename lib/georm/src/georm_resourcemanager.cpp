//
// Created by jlemein on 21-03-21.
//
#include <georm_resourcemanager.h>

#include <geo_scene.h>
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <georm_exrloader.h>
#include <georm_materialsystem.h>
#include <georm_sceneloader.h>
#include <georm_texturesystem.h>

using namespace lsw;
using namespace lsw::georm;

namespace {
std::once_flag getTexturesCall;
}

ResourceManager::ResourceManager() {}

void ResourceManager::setMaterialSystem(std::shared_ptr<georm::MaterialSystem> materialSystem) {
  m_materialSystem = materialSystem;
}

void ResourceManager::setTextureSystem(std::shared_ptr<georm::TextureSystem> textureSystem) {
  m_textureSystem = textureSystem;
}

void ResourceManager::setSceneLoader(std::shared_ptr<georm::SceneLoader> sceneLoader) {
  m_sceneLoader = sceneLoader;
}

std::shared_ptr<georm::MaterialSystem> ResourceManager::getMaterialSystem() {
  return m_materialSystem;
}

std::shared_ptr<georm::TextureSystem> ResourceManager::getTextureSystem() {
  return m_textureSystem;
}

std::shared_ptr<georm::SceneLoader> ResourceManager::getSceneLoader() {
  return m_sceneLoader;
}
