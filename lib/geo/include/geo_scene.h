//
// Created by jlemein on 01-02-21.
//
#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <list>
#include <vector>
#include <unordered_map>

namespace lsw {
namespace geo {

struct MeshInstance;
struct Light;
struct Camera;
struct Mesh;
struct Texture;

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
  //    friend class SceneLoader;

 public:
  using MeshIterable = std::vector<std::shared_ptr<Mesh>>;
  //  using MeshInstanceIterable = std::vector<std::shared_ptr<MeshInstance>>;
  using TextureIterable = std::vector<std::shared_ptr<Texture>>;
  using MaterialIterable = std::vector<int>;
  using LightIterable = std::vector<std::shared_ptr<Light>>;
  using CameraIterable = std::vector<std::shared_ptr<Camera>>;
  using SceneNodeIterable = std::vector<std::shared_ptr<SceneNode>>;

  MeshIterable& meshes() {
    return m_meshes;
  }
  //  MeshInstanceIterable meshInstances() { return m_instances; }
  //  TextureIterable textures() { return m_textures; }
  MaterialIterable materials() {
    return m_materials;
  }

  LightIterable lights() {
    return m_lights;
  }
  CameraIterable cameras() {
    return m_cameras;
  }

  std::shared_ptr<const SceneNode> rootNode() const {
    return m_rootNode;
  }
  std::shared_ptr<SceneNode> rootNode() {
    return m_rootNode;
  }

  SceneNodeIterable getSceneNodesByName(const std::string& name);

  // private:
  std::filesystem::path m_path;  /// @brief Base path of the scene file. Resources shall be loaded relative from this path.
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

}  // namespace geo
}  // namespace lsw
