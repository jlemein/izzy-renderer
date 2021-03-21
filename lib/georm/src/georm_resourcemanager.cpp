//
// Created by jlemein on 21-03-21.
//
#include <georm_resourcemanager.h>
#include <geo_materialloader.h>
#include <res_resourcemanager.h>
#include <geo_scene.h>
#include <geo_sceneloader.h>
#include <geo_texture.h>
#include <geo_textureloader.h>
using namespace lsw;
using namespace affx;
using namespace lsw::georm;

ResourceManager::ResourceManager(const std::string& materialsFile)
: m_wrappedResourceMgr(std::make_shared<res::ResourceManager>())
{
  auto materialSystem = std::make_unique<geo::MaterialSystem>(materialsFile);
  materialSystem->initialize();

  m_wrappedResourceMgr->addFactory<geo::Material>(move(materialSystem));
  m_wrappedResourceMgr->addFactory<affx::geo::Scene>(
      std::make_unique<geo::SceneLoader>(m_wrappedResourceMgr));
  m_wrappedResourceMgr->addFactory<geo::Texture>(std::make_unique<geo::TextureLoader>());
}

std::shared_ptr<affx::res::ResourceManager> ResourceManager::getRawResourceManager() {
  return m_wrappedResourceMgr;
}

const geo::Material& ResourceManager::loadMaterial(const std::string& name) {
  return **m_wrappedResourceMgr->getResource<geo::Material>(name);
}

const affx::geo::Texture& ResourceManager::loadTexture(const std::string& path) {
  return **m_wrappedResourceMgr->getResource<geo::Texture>(path);
}