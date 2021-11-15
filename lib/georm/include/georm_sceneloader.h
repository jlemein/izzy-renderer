//
// Created by jlemein on 18-01-21.
//
#pragma once

#include <assimp/scene.h>
#include <geo_transform.h>
#include <glm/glm.hpp>
#include <list>
#include <res_resourcefactory.h>
#include <vector>
#include <memory>
#include <filesystem>

namespace lsw {
namespace res {
class ResourceManager;
template <typename T> class Resource;
}
namespace geo {
    struct Scene;
    struct Texture;
    struct Material;
} // namespace geo

namespace georm {

class ResourceManager;

class SceneLoader : public lsw::res::ResourceFactory {
public:
  SceneLoader(ResourceManager* resourceManager);

  std::unique_ptr<res::IResource> createResource(const std::string &path) override;

private:
  ResourceManager* m_resourceManager {nullptr};

  void readMaterials(const aiScene *aiScene, geo::Scene &scene);
  void readMeshes(const aiScene *aiScene, geo::Scene &scene);
  void readHierarchy(const aiScene *aiScene, geo::Scene &scene);
  void readLights(const aiScene *aiScene, geo::Scene &scene);
  void readCameras(const aiScene *aiScene, geo::Scene &scene);

  /**
   * @brief Reads the textures associated to the material. If material system
   * contains an entry for the material, they take precedence over the textures
   * defined in the scene file.
   * @param scene Scene file that is currently being processed. Passed because you might need to resolve relative paths.
   * @param aiMaterial
   * @param material
   */
  void readTextures(const geo::Scene &scene, const aiMaterial* aiMaterial, geo::Material& material);

  std::shared_ptr<geo::Texture> readDiffuseTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<geo::Texture> readSpecularTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<geo::Texture> readNormalTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<geo::Texture> readRoughnessTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;

//  void readEmbeddedTextures(const aiScene* aiScene, geo::Scene& scene);
};

} // namespace georm
} // namespace lsw
