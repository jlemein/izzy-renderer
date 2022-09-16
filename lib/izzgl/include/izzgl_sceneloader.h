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

namespace lsw {
namespace geo {
struct Scene;
}  // namespace geo
}  // namespace lsw

namespace izz {
namespace geo {
struct MaterialDescription;
struct TextureDescription;
}  // namespace geo
namespace gl {
class MaterialSystem;
class TextureSystem;

/**
 * @brief Scene loader loads a scene file
 */
class SceneLoader {
 public:
  /**
   * Constructor
   * @param textureSystem needed to resolve and load textures used in the scene file.
   * @param materialSystem needed to resolve material names used in the scene file.
   */
  SceneLoader(std::shared_ptr<izz::gl::TextureSystem> textureSystem, std::shared_ptr<izz::gl::MaterialSystem> materialSystem);

  /**
   * Loads a 3D scene from disk, such as an *.fbx file.
   * @param path File path to the scene file.
   * @return A unique pointer to the scene file.
   */
  std::unique_ptr<lsw::geo::Scene> loadScene(std::filesystem::path path);

 private:
  /**
   * Reads materials from assimp scene, and adds the materials to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read materials from assimp scene.
   */
  void readMaterials(const aiScene* aiScene, lsw::geo::Scene& scene);

  /**
   * Reads meshes from assimp scene, and adds the meshes to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read meshes from assimp scene.
   */
  void readMeshes(const aiScene* aiScene, lsw::geo::Scene& scene);

  /**
   * Reads hierarchy from assimp scene, and adds the hierarchy to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read hierarchy from assimp scene.
   */
  void readHierarchy(const aiScene* aiScene, lsw::geo::Scene& scene);

  /**
   * Reads lights from assimp scene, and adds the lights to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read lights from assimp scene.
   */
  void readLights(const aiScene* aiScene, lsw::geo::Scene& scene);

  /**
   * Reads cameras from assimp scene, and adds the cameras to the scene reference.
   * @param aiScene Assimp scene
   * @param scene Scene that will contain the read cameras from assimp scene.
   */
  void readCameras(const aiScene* aiScene, lsw::geo::Scene& scene);

  /**
   * @brief Reads the textures associated to the material. If material system
   * contains an entry for the material, they take precedence over the textures
   * defined in the scene file.
   * @param scene Scene file that is currently being processed. Passed because you might need to resolve relative paths.
   * @param aiMaterial
   * @param material
   */
  void readTextures(const lsw::geo::Scene& scene, const aiMaterial* aiMaterial, izz::geo::MaterialDescription& material);

  std::unique_ptr<izz::geo::TextureDescription> readAiTexture(const lsw::geo::Scene& scene, aiTextureType ttype, const aiMaterial* aiMaterial) const;

  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem{nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
};

}  // namespace gl
}  // namespace izz
