//
// Created by jlemein on 25-02-21.
//

#ifndef RENDERER_GEORM_RESOURCEMANAGER_H
#define RENDERER_GEORM_RESOURCEMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <res_resource.h>

namespace lsw {
namespace res {
class ResourceFactory;
class ResourceManager;
} // namespace res

namespace geo {
class Material;
class Texture;
}
} // namespace lsw

namespace lsw {
namespace georm {

using MaterialPtr = std::shared_ptr<res::Resource<geo::Material>>;
using TexturePtr = std::shared_ptr<res::Resource<geo::Texture>>;

/**!
 * @brief Higher level resource manager specifically focused on loading
 * entities from the geometry package. The geometry package contains all higher
 * level entities, independent of the rendering system used.
 */
class ResourceManager {
public:
  ResourceManager(const std::string& materialsFile);

  MaterialPtr createSharedMaterial(const std::string& name);
  geo::Material createMaterial(const std::string& name);

  TexturePtr loadTexture(const std::string& path);

  std::shared_ptr<res::ResourceManager> getRawResourceManager();

private:
  // TODO: must be nullptr
  std::shared_ptr<res::ResourceManager> m_wrappedResourceMgr {nullptr};
  std::unordered_map<std::string, MaterialPtr> m_cachedMaterials;
  std::unordered_map<std::string, TexturePtr> m_cachedTextures;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEORM_RESOURCEMANAGER_H
