//
// Created by jlemein on 25-02-21.
//

#ifndef RENDERER_GEORM_RESOURCEMANAGER_H
#define RENDERER_GEORM_RESOURCEMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

namespace lsw {
namespace res {
class ResourceManager;
} // namespace res

namespace geo {
class Material;
class Texture;
}
} // namespace lsw

namespace lsw {
namespace georm {

/**!
 * @brief Higher level resource manager specifically focused on loading
 * entities from the geometry package. The geometry package contains all higher
 * level entities, independent of the rendering system used.
 */
class ResourceManager {
public:
  ResourceManager(const std::string& materialsFile);

  lsw::geo::Material& loadMaterial(const std::string& name);
  lsw::geo::Texture& loadTexture(const std::string& path);

  std::shared_ptr<lsw::res::ResourceManager> getRawResourceManager();

private:
  // TODO: must be nullptr
  std::shared_ptr<lsw::res::ResourceManager> m_wrappedResourceMgr {nullptr};
  std::unordered_map<std::string, std::shared_ptr<res::Resource<geo::Material>>> m_cachedResources;
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEORM_RESOURCEMANAGER_H
