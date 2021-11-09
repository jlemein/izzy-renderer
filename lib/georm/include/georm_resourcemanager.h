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
using MaterialPtr = std::shared_ptr<res::Resource<geo::Material>>;
using TexturePtr = std::shared_ptr<res::Resource<geo::Texture>>;
using ScenePtr = std::shared_ptr<res::Resource<geo::Scene>>;

/**!
 * @brief Higher level resource manager specifically focused on loading
 * entities from the geometry package. The geometry package contains all higher
 * level entities, independent of the rendering system used.
 */
class ResourceManager {
public:
  ResourceManager();

  void setMaterialSystem(std::shared_ptr<georm::MaterialSystem> materialSystem);
  std::shared_ptr<georm::MaterialSystem> getMaterialSystem();

  MaterialPtr createSharedMaterial(const std::string& name);
  geo::Material createMaterial(const std::string& name);

  TexturePtr loadTexture(const std::string& path);

  /** @brief load a scene file
   *
   * @param path Path to scene file (i.e. *.fbx, *.obj, etc).
   * @return
   */
  ScenePtr loadScene(std::filesystem::path path);

  std::shared_ptr<res::ResourceManager> getRawResourceManager();

  const std::unordered_map<std::string, TexturePtr>& getTextures() const;

private:
  std::shared_ptr<res::ResourceManager> m_wrappedResourceMgr {nullptr};
  std::shared_ptr<georm::MaterialSystem> m_materialSystem {nullptr};

  std::unordered_map<std::string, MaterialPtr> m_cachedMaterials;
  std::unordered_map<std::string, TexturePtr> m_cachedTextures;
  std::unordered_map<std::string, ScenePtr> m_loadedScenes;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEORM_RESOURCEMANAGER_H
