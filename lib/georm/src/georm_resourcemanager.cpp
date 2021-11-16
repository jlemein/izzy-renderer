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
#include <res_resourcemanager.h>

using namespace lsw;
using namespace lsw::georm;

namespace {
std::once_flag getTexturesCall;
}

ResourceManager::ResourceManager()
  : m_wrappedResourceMgr(std::make_shared<res::ResourceManager>()) {
}

void ResourceManager::setMaterialSystem(std::shared_ptr<georm::MaterialSystem> materialSystem) {
  m_materialSystem = materialSystem;
}

void ResourceManager::setTextureSystem(std::shared_ptr<georm::TextureSystem> textureSystem) {
  m_textureSystem = textureSystem;
}

void ResourceManager::setSceneLoader(std::shared_ptr<georm::SceneLoader> sceneLoader) {
  m_sceneLoader = sceneLoader;
  m_wrappedResourceMgr->addFactory<geo::Scene>(sceneLoader);
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

std::shared_ptr<lsw::res::ResourceManager> ResourceManager::getRawResourceManager() {
  return m_wrappedResourceMgr;
}

ScenePtr ResourceManager::loadScene(std::filesystem::path path) {
  if (m_loadedScenes.find(path) == m_loadedScenes.end()) {
    m_loadedScenes[path] = m_wrappedResourceMgr->createResource<geo::Scene>(path);
  }
  return m_loadedScenes.at(path);
}

MaterialPtr ResourceManager::createMaterial(const std::string& name) {
  return m_materialSystem->createMaterial(name);
}

const std::unordered_map<std::string, TexturePtr> ResourceManager::getTextures() const {
  std::call_once(getTexturesCall, [] { spdlog::warn("Not implemented: ResourceManager::getTextures()"); });

  //    return m_textureSystem->getTextures();
  return std::unordered_map<std::string, TexturePtr>{};
}

TexturePtr ResourceManager::loadTexture(const std::filesystem::path& path) {
  return m_textureSystem->loadTexture(path);
}