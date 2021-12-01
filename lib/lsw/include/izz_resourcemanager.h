//
// Created by jlemein on 25-02-21.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace lsw {
namespace geo {
class Material;
class Texture;
class Scene;
}

class MaterialSystem;
class SceneLoader;
class TextureSystem;

/**!
 * @brief Higher level resource manager specifically focused on loading
 * entities from the geometry package. The geometry package contains all higher
 * level entities, independent of the rendering system used.
 */
class ResourceManager : public std::enable_shared_from_this<ResourceManager> {
public:
  ResourceManager();

  /**
   * Sets the material system
   * @param materialSystem
   */
  void setMaterialSystem(std::shared_ptr<MaterialSystem> materialSystem);
  void setTextureSystem(std::shared_ptr<TextureSystem> textureSystem);
  void setSceneLoader(std::shared_ptr<SceneLoader> sceneLoader);

  std::shared_ptr<MaterialSystem> getMaterialSystem();
  std::shared_ptr<TextureSystem> getTextureSystem();
  std::shared_ptr<SceneLoader> getSceneLoader();

private:
  std::shared_ptr<MaterialSystem> m_materialSystem {nullptr};
  std::shared_ptr<SceneLoader> m_sceneLoader {nullptr};
  std::shared_ptr<TextureSystem> m_textureSystem;
};

} // lsw
