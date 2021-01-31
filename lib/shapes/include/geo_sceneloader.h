//
// Created by jlemein on 18-01-21.
//

#ifndef RENDERER_GEO_SCENELOADER_H
#define RENDERER_GEO_SCENELOADER_H

#include <res_resourcefactory.h>
#include <assimp/scene.h>
#include <vector>
#include <geo_transform.h>

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
  Transform<float> transform;
  std::vector<std::shared_ptr<MeshInstance>> meshInstances {};
  std::vector<unsigned int> children {};
};

class Scene {
  friend class SceneLoader;
public:
  using MeshIterable = std::vector<std::shared_ptr<Mesh>>;
  using MeshInstanceIterable = std::vector<std::shared_ptr<MeshInstance>>;
  using TextureIterable = std::vector<std::shared_ptr<Texture>>;
  using MaterialIterable = std::vector<Material>;
  using NodeHierarchy = std::vector<SceneNode>;

  MeshIterable& meshes() { return m_meshes; }
  MeshInstanceIterable meshInstances() { return m_instances; }
//  TextureIterable textures() { return m_textures; }
//  MaterialIterable materials() { return m_materials; }
//  NodeHierarchy nodeHierarchy() {return m_nodeHierarchy; }

private:
  MeshIterable m_meshes {};
  MeshInstanceIterable m_instances{};
  NodeHierarchy m_nodeHierarchy {};
};

class SceneLoader : public afxx::res::ResourceFactory {
public:
  std::shared_ptr<void> loadResource(const std::string &path) override;

private:
  void readMeshes(const aiScene* aiScene, geo::Scene& scene);
  void readInstances(const aiScene* aiScene, geo::Scene& scene);
//  void readLights(const aiScene* aiScene, geo::Scene& scene);
//  void readTextures(const aiScene* aiScene, geo::Scene& scene);
//  void readCameras(const aiScene* aiScene, geo::Scene& scene);
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_SCENELOADER_H
