//
// Created by jlemein on 21-03-21.
//
#include <izz_resourcemanager.h>

#include "geo_scene.h"
#include "izzgl_exrloader.h"
#include "izzgl_materialsystem.h"
#include "izzgl_sceneloader.h"
#include "izzgl_texture.h"
#include "izzgl_textureloader.h"
#include "izzgl_texturesystem.h"

using namespace lsw;
using namespace izz;

ResourceManager::ResourceManager() {}

void ResourceManager::setMaterialSystem(std::shared_ptr<gl::MaterialSystem> materialSystem) {
  m_materialSystem = materialSystem;
}

void ResourceManager::setTextureSystem(std::shared_ptr<izz::gl::TextureSystem> textureSystem) {
  m_textureSystem = textureSystem;
}

void ResourceManager::setSceneLoader(std::shared_ptr<izz::gl::SceneLoader> sceneLoader) {
  m_sceneLoader = sceneLoader;
}

std::shared_ptr<gl::MaterialSystem> ResourceManager::getMaterialSystem() {
  return m_materialSystem;
}

std::shared_ptr<izz::gl::TextureSystem> ResourceManager::getTextureSystem() {
  return m_textureSystem;
}

std::shared_ptr<izz::gl::SceneLoader> ResourceManager::getSceneLoader() {
  return m_sceneLoader;
}
