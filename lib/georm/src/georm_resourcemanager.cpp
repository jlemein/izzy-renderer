//
// Created by jlemein on 21-03-21.
//
#include <georm_materialsystem.h>
#include <geo_scene.h>
#include <georm_sceneloader.h>
#include <geo_texture.h>
#include <geo_textureloader.h>
#include <georm_resourcemanager.h>
#include <georm_sceneloader.h>
#include <georm_exrloader.h>

#include <res_resourcemanager.h>

using namespace lsw;
using namespace lsw::georm;

namespace {
    std::once_flag getTexturesCall;
}

ResourceManager::ResourceManager()
        : m_wrappedResourceMgr(std::make_shared<res::ResourceManager>()) {

//  m_wrappedResourceMgr->addFactory<geo::Material>(move(materialFactory));

    //TODO: tricky situation. Verify best approach for scene loader and reference to this.
    m_wrappedResourceMgr->addFactory<lsw::geo::Scene>(
            std::make_unique<SceneLoader>(this));
    m_wrappedResourceMgr->addFactory<geo::Texture>(
            std::make_unique<geo::TextureLoader>());

    m_textureSystem.setTextureLoader(".exr", std::make_unique<ExrLoader>());
//    m_textureSystem.setTextureLoader(geo::ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<ExrLoader>());
}

void ResourceManager::setMaterialSystem(std::shared_ptr<georm::MaterialSystem> materialSystem) {
    m_materialSystem = materialSystem;
    m_wrappedResourceMgr->addFactory<geo::Material>(materialSystem);
}

std::shared_ptr<lsw::res::ResourceManager>
ResourceManager::getRawResourceManager() {
    return m_wrappedResourceMgr;
}

ScenePtr ResourceManager::loadScene(std::filesystem::path path) {
    if (m_loadedScenes.find(path) == m_loadedScenes.end()) {
        m_loadedScenes[path] =
                m_wrappedResourceMgr->createResource<geo::Scene>(path);
    }
    return m_loadedScenes.at(path);
}

MaterialPtr ResourceManager::createSharedMaterial(const std::string &name) {
    if (m_cachedMaterials.find(name) == m_cachedMaterials.end()) {
        m_cachedMaterials[name] =
                m_wrappedResourceMgr->createResource<geo::Material>(name);
    }
    return m_cachedMaterials.at(name);
}

geo::Material ResourceManager::createMaterial(const std::string &name) {
    // TODO: there is a possibility to alter the material before creation. Maybe it is what we want.
    return **createSharedMaterial(name);
}

const std::unordered_map<std::string, TexturePtr> ResourceManager::getTextures() const {
    std::call_once(getTexturesCall, [] {
        spdlog::warn("Not implemented: ResourceManager::getTextures()");
    });

    //    return m_textureSystem->getTextures();
    return std::unordered_map<std::string, TexturePtr>{};
}

TexturePtr ResourceManager::loadTexture(const std::filesystem::path &path) {
    return m_textureSystem.loadTexture(path);
}