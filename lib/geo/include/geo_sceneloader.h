//
// Created by jlemein on 18-01-21.
//

#ifndef RENDERER_GEO_SCENELOADER_H
#define RENDERER_GEO_SCENELOADER_H

#include "geo_camera.h"
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

struct Mesh;
struct MeshInstance;
struct Texture;
struct Light;
struct Material;
struct SceneNode;
class MaterialSystem;

/**!
 * @brief A scene node is either representing an intermediate transform node,
 * or it represents a mesh instance.
 */
struct SceneNode {
  std::string name;
  glm::mat4 transform{glm::mat4(1.0F)};
  std::vector<std::shared_ptr<MeshInstance>> meshInstances{};
  std::list<std::shared_ptr<SceneNode>> children{};
  std::vector<std::shared_ptr<Light>> lights{};
  std::vector<std::shared_ptr<Camera>> cameras{};
};

class Scene {
  friend class SceneLoader;

public:
  using MeshIterable = std::vector<std::shared_ptr<Mesh>>;
  //  using MeshInstanceIterable = std::vector<std::shared_ptr<MeshInstance>>;
  using TextureIterable = std::vector<std::shared_ptr<Texture>>;
  using MaterialIterable = std::vector<std::shared_ptr<res::Resource<geo::Material>>>;
  using LightIterable = std::vector<std::shared_ptr<Light>>;
  using CameraIterable = std::vector<std::shared_ptr<Camera>>;
  using SceneNodeIterable = std::vector<std::shared_ptr<SceneNode>>;

  MeshIterable &meshes() { return m_meshes; }
  //  MeshInstanceIterable meshInstances() { return m_instances; }
  //  TextureIterable textures() { return m_textures; }
  MaterialIterable materials() { return m_materials; }

  LightIterable lights() { return m_lights; }
  CameraIterable cameras() { return m_cameras; }

  std::shared_ptr<SceneNode> rootNode() { return m_rootNode; }

  SceneNodeIterable getSceneNodesByName(const std::string &name);

private:
  std::filesystem::path m_path; /// @brief Base path of the scene file. Resources shall be loaded relative from this path.
  std::filesystem::path m_dir;

  std::unordered_map<std::string, SceneNodeIterable> m_sceneNodes;
  std::shared_ptr<SceneNode> m_rootNode{nullptr};
  MaterialIterable m_materials{};
  MeshIterable m_meshes{};
  LightIterable m_lights{};
  CameraIterable m_cameras{};

  // Registers the name with a scene node.
  // The same name can map to multiple nodes.
  void registerSceneNode(std::shared_ptr<SceneNode> node);

  // TODO write custom iterator to iterate over mesh instances
  //  MeshInstanceIterable m_instances{};
};

class SceneLoader : public lsw::res::ResourceFactory {
public:
  SceneLoader(std::shared_ptr<res::ResourceManager> resourceManager);

  std::unique_ptr<res::IResource> createResource(const std::string &path) override;

private:
  std::shared_ptr<res::ResourceManager> m_resourceManager {nullptr};

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

  std::shared_ptr<res::Resource<geo::Texture>> readDiffuseTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<res::Resource<geo::Texture>> readSpecularTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<res::Resource<geo::Texture>> readNormalTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;
  std::shared_ptr<res::Resource<geo::Texture>> readRoughnessTexture(const geo::Scene &scene, const aiMaterial* aiMaterial, const geo::Material& material) const;

//  void readEmbeddedTextures(const aiScene* aiScene, geo::Scene& scene);
};

} // namespace geo
} // namespace lsw

#endif // RENDERER_GEO_SCENELOADER_H
