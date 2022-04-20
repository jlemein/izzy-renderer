//
// Created by jlemein on 18-01-21.
//
#pragma once

#include <assimp/scene.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <vector>
#include "geo_transform.h"

namespace izz {
namespace gl {
class MaterialSystem;
class TextureSystem;
struct Texture;
struct Material;
}  // namespace gl
}  // namespace izz

namespace lsw {
namespace res {

template <typename T>
class Resource;
}  // namespace res

namespace geo {
struct Scene;
}  // namespace geo

class SceneLoader {
 public:
  SceneLoader(std::shared_ptr<izz::gl::TextureSystem> textureSystem, std::shared_ptr<izz::gl::MaterialSystem> materialSystem);

  std::shared_ptr<geo::Scene> loadScene(const std::string& path);

 private:
  /**
   * Reads materials from assimp scene, and adds the materials to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read materials from assimp scene.
   */
  void readMaterials(const aiScene* aiScene, geo::Scene& scene);

  /**
   * Reads meshes from assimp scene, and adds the meshes to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read meshes from assimp scene.
   */
  void readMeshes(const aiScene* aiScene, geo::Scene& scene);

  /**
   * Reads hierarchy from assimp scene, and adds the hierarchy to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read hierarchy from assimp scene.
   */
  void readHierarchy(const aiScene* aiScene, geo::Scene& scene);

  /**
   * Reads lights from assimp scene, and adds the lights to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read lights from assimp scene.
   */
  void readLights(const aiScene* aiScene, geo::Scene& scene);

  /**
   * Reads cameras from assimp scene, and adds the cameras to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read cameras from assimp scene.
   */
  void readCameras(const aiScene* aiScene, geo::Scene& scene);

  /**
   * @brief Reads the textures associated to the material. If material system
   * contains an entry for the material, they take precedence over the textures
   * defined in the scene file.
   * @param scene Scene file that is currently being processed. Passed because you might need to resolve relative paths.
   * @param aiMaterial
   * @param material
   */
  void readTextures(const geo::Scene& scene, const aiMaterial* aiMaterial, izz::gl::Material& material);

  izz::gl::Texture* readDiffuseTexture(const geo::Scene& scene, const aiMaterial* aiMaterial, const izz::gl::Material& material) const;
  izz::gl::Texture* readSpecularTexture(const geo::Scene& scene, const aiMaterial* aiMaterial, const izz::gl::Material& material) const;
  izz::gl::Texture* readNormalTexture(const geo::Scene& scene, const aiMaterial* aiMaterial, const izz::gl::Material& material) const;
  izz::gl::Texture* readRoughnessTexture(const geo::Scene& scene, const aiMaterial* aiMaterial, const izz::gl::Material& material) const;

  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem{nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
};

}  // namespace lsw
