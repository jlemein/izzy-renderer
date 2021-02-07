//
// Created by jlemein on 18-01-21.
//

#ifndef RENDERER_GEO_SCENELOADER_H
#define RENDERER_GEO_SCENELOADER_H

#include <res_resourcefactory.h>
#include <assimp/scene.h>
#include <vector>
#include <list>
#include <geo_transform.h>
#include <glm/glm.hpp>

namespace affx {
namespace geo {

struct Mesh;
struct MeshInstance;
struct Texture;
struct Material;
struct SceneNode;

/**!
 * @brief A scene node is either representing an intermediate transform node,
 * or it represents a mesh instance.
 */
struct SceneNode {
  std::string name;
  glm::mat4 transform {glm::mat4(1.0F)};
  std::vector<std::shared_ptr<MeshInstance>> meshInstances {};
  std::list<std::shared_ptr<SceneNode>> children {};
};

class Scene {
  friend class SceneLoader;
public:
  using MeshIterable = std::vector<std::shared_ptr<Mesh>>;
  using MeshInstanceIterable = std::vector<std::shared_ptr<MeshInstance>>;
  using TextureIterable = std::vector<std::shared_ptr<Texture>>;
  using MaterialIterable = std::vector<std::shared_ptr<Material>>;

  MeshIterable& meshes() { return m_meshes; }
//  MeshInstanceIterable meshInstances() { return m_instances; }
//  TextureIterable textures() { return m_textures; }
  MaterialIterable materials() { return m_materials; }
  std::shared_ptr<SceneNode> rootNode() {return m_rootNode; }

private:
  MaterialIterable m_materials{};
  MeshIterable m_meshes {};
  std::shared_ptr<SceneNode> m_rootNode {nullptr};

  //TODO write custom iterator to iterate over mesh instances
  //  MeshInstanceIterable m_instances{};
};

class SceneLoader : public affx::res::ResourceFactory {
public:
  std::shared_ptr<void> loadResource(const std::string &path) override;

private:
  void readMaterials(const aiScene* aiScene, geo::Scene& scene);
  void readMeshes(const aiScene* aiScene, geo::Scene& scene);
  void readHierarchy(const aiScene* aiScene, geo::Scene& scene);
//  void readLights(const aiScene* aiScene, geo::Scene& scene);
//  void readTextures(const aiScene* aiScene, geo::Scene& scene);
//  void readCameras(const aiScene* aiScene, geo::Scene& scene);
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_SCENELOADER_H
