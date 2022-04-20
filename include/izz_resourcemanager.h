//
// Created by jlemein on 25-02-21.
//
#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace izz {
namespace gl {
class MaterialSystem;
class TextureSystem;
}  // namespace gl
}  // namespace izz
namespace lsw {
namespace geo {
class Texture;
class Scene;
}  // namespace geo

class SceneLoader;

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
  void setMaterialSystem(std::shared_ptr<izz::gl::MaterialSystem> materialSystem);
  void setTextureSystem(std::shared_ptr<izz::gl::TextureSystem> textureSystem);
  void setSceneLoader(std::shared_ptr<SceneLoader> sceneLoader);

  std::shared_ptr<izz::gl::MaterialSystem> getMaterialSystem();
  std::shared_ptr<izz::gl::TextureSystem> getTextureSystem();
  std::shared_ptr<SceneLoader> getSceneLoader();

 private:
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
  std::shared_ptr<SceneLoader> m_sceneLoader{nullptr};
  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem;
};

}  // namespace lsw
