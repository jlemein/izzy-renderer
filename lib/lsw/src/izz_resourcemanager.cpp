//
// Created by jlemein on 21-03-21.
//
#include <izz_resourcemanager.h>

#include <geo_scene.h>
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <izz_exrloader.h>
#include <izz_materialsystem.h>
#include <izz_sceneloader.h>
#include <izz_texturesystem.h>

using namespace lsw;

namespace {
std::once_flag getTexturesCall;
}

ResourceManager::ResourceManager() {}

void ResourceManager::setMaterialSystem(std::shared_ptr<MaterialSystem> materialSystem) {
  m_materialSystem = materialSystem;
}

void ResourceManager::setTextureSystem(std::shared_ptr<TextureSystem> textureSystem) {
  m_textureSystem = textureSystem;
}

void ResourceManager::setSceneLoader(std::shared_ptr<SceneLoader> sceneLoader) {
  m_sceneLoader = sceneLoader;
}

std::shared_ptr<MaterialSystem> ResourceManager::getMaterialSystem() {
  return m_materialSystem;
}

std::shared_ptr<TextureSystem> ResourceManager::getTextureSystem() {
  return m_textureSystem;
}

std::shared_ptr<SceneLoader> ResourceManager::getSceneLoader() {
  return m_sceneLoader;
}
