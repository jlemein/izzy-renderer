//
// Created by jlemein on 25-02-21.
//

#ifndef RENDERER_GEORM_RESOURCEMANAGER_H
#define RENDERER_GEORM_RESOURCEMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <res_resource.h>
#include "georm_texturesystem.h"

namespace lsw {
namespace res {
class ResourceFactory;
class ResourceManager;
} // namespace res

namespace geo {
class Material;
class Texture;
class Scene;
}
} // namespace lsw

namespace lsw {
namespace georm {

class MaterialSystem;
using MaterialPtr = std::shared_ptr<geo::Material>;
using TexturePtr = std::shared_ptr<geo::Texture>;
using ScenePtr = std::shared_ptr<res::Resource<geo::Scene>>;
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
  void setMaterialSystem(std::shared_ptr<MaterialSystem> materialSystem);
  void setTextureSystem(std::shared_ptr<TextureSystem> textureSystem);
  void setSceneLoader(std::shared_ptr<SceneLoader> sceneLoader);

  std::shared_ptr<MaterialSystem> getMaterialSystem();
  std::shared_ptr<TextureSystem> getTextureSystem();
  std::shared_ptr<SceneLoader> getSceneLoader();

  MaterialPtr createMaterial(const std::string& name);
  TexturePtr loadTexture(const std::filesystem::path& path);
  ScenePtr loadScene(std::filesystem::path path);
  std::shared_ptr<res::ResourceManager> getRawResourceManager();
  const std::unordered_map<std::string, TexturePtr> getTextures() const;

private:
  std::shared_ptr<res::ResourceManager> m_wrappedResourceMgr {nullptr};
  std::shared_ptr<georm::MaterialSystem> m_materialSystem {nullptr};
  std::shared_ptr<georm::SceneLoader> m_sceneLoader {nullptr};
  std::shared_ptr<georm::TextureSystem> m_textureSystem;

  std::unordered_map<std::string, ScenePtr> m_loadedScenes;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEORM_RESOURCEMANAGER_H
