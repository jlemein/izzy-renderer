//
// Created by jlemein on 25-02-21.
//

#ifndef RENDERER_GEORM_RESOURCEMANAGER_H
#define RENDERER_GEORM_RESOURCEMANAGER_H

#include <memory>
#include <string>

namespace affx {
namespace res {
class ResourceManager;
} // namespace res

namespace geo {
class Material;
class Texture;
}
} // namespace affx

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

  const affx::geo::Material& loadMaterial(const std::string& name);
  const affx::geo::Texture& loadTexture(const std::string& path);

  std::shared_ptr<affx::res::ResourceManager> getRawResourceManager();

private:
  // TODO: must be nullptr
  std::shared_ptr<affx::res::ResourceManager> m_wrappedResourceMgr {nullptr};
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEORM_RESOURCEMANAGER_H
