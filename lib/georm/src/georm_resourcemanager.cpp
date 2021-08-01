//
// Created by jlemein on 21-03-21.
//
#include <geo_materialloader.h>
#include <geo_scene.h>
#include <geo_sceneloader.h>
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <georm_resourcemanager.h>
#include <res_resourcemanager.h>
using namespace lsw;
using namespace lsw;
using namespace lsw::georm;

ResourceManager::ResourceManager(const std::string &materialsFile)
    : m_wrappedResourceMgr(std::make_shared<res::ResourceManager>()) {
  auto materialSystem = std::make_unique<geo::MaterialSystem>(materialsFile);
  materialSystem->initialize();

  m_wrappedResourceMgr->addFactory<geo::Material>(move(materialSystem));
  m_wrappedResourceMgr->addFactory<lsw::geo::Scene>(
      std::make_unique<geo::SceneLoader>(m_wrappedResourceMgr));
  m_wrappedResourceMgr->addFactory<geo::Texture>(
      std::make_unique<geo::TextureLoader>());
}

std::shared_ptr<lsw::res::ResourceManager>
ResourceManager::getRawResourceManager() {
  return m_wrappedResourceMgr;
}

geo::Material &ResourceManager::loadMaterial(const std::string &name) {
  if (m_cachedResources.find(name) == m_cachedResources.end()) {
    m_cachedResources[name] =
        m_wrappedResourceMgr->getResource<geo::Material>(name);
  }
  return **m_cachedResources.at(name);
}

lsw::geo::Texture &ResourceManager::loadTexture(const std::string &path) {
  return **m_wrappedResourceMgr->getResource<geo::Texture>(path);
}