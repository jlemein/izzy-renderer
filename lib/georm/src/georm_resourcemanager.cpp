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

MaterialPtr ResourceManager::createSharedMaterial(const std::string &name) {
  if (m_cachedMaterials.find(name) == m_cachedMaterials.end()) {
    m_cachedMaterials[name] =
        m_wrappedResourceMgr->createResource<geo::Material>(name);
  }
  return m_cachedMaterials.at(name);
}

geo::Material ResourceManager::createMaterial(const std::string& name) {
  // TODO: there is a possibility to alter the material before creation. Maybe it is what we want.
  return **createSharedMaterial(name);
}

TexturePtr ResourceManager::loadTexture(const std::string &path) {
  if (m_cachedTextures.find(path) == m_cachedTextures.end()) {
    m_cachedTextures[path] =
        m_wrappedResourceMgr->createResource<geo::Texture>(path);
  }
  return m_cachedTextures.at(path);
}