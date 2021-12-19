//
// Created by jlemein on 05-08-21.
//
#pragma once

#include <izz_resourcemanager.h>
#include <entt/entt.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include "geo_material.h"
#include "glrs_rendersystem.h"
#include "izz_scenegraphentity.h"
#include "uniform_uniformblockmanager.h"

namespace lsw {

class ResourceManager;

}  // namespace lsw

namespace izz {
class SceneGraph;
namespace gl {

/**
 * The material system manages the material components in the scene.
 * It is responsible for reading the material definitions (json) file that list
 * the available materials.
 *
 * Every material described in the materials file should be mapped to a data
 * structure.
 */
class MaterialSystem : public lsw::glrs::IMaterialSystem {
 public:
  MaterialSystem(std::shared_ptr<izz::SceneGraph> sceneGraph, std::shared_ptr<lsw::ResourceManager> resourceManager);

  virtual ~MaterialSystem() = default;

  void loadMaterialsFromFile(const std::filesystem::path& path);

  /**
   * @brief Creates a new material associated with the specified material name.
   * A new material means that there is no relationship between different instances
   * using the same material name.
   *
   * @details the material is loaded based on material name. There are four
   * possibilities:
   *  1. The provided material name is mapped to a different material name
   *  (based on material mapping).
   *  2. If the provided material name is not a key in the material map, a
   *  direct material lookup is performed.
   *  3. If not found, then the default material is used.
   *  4. If there is no default material, an exception is thrown.
   * @returns a unique ptr
   */
  std::shared_ptr<lsw::geo::Material> createMaterial(const std::string& name);

  /**
   * Every frame this method is called to let the material system update it's shader parameters.
   * @param time Global simulation time.
   * @param dt Delta time passed since previous frame.
   */
  void update(float time, float dt) override;

  /**
   * Loops over all materials in the scene graph and makes sure the textures are loaded (if not loaded already).
   * Also it synchronizes the render system with the entities' texture data (by attaching or detaching texture data).
   *
   * @param renderSystem Handle to the render system to instruct to synch a texture.
   * @details
   * In essence, it makes sure the textures are loaded or removed.
   * it makes sure the rendersystem is updated with new texture data, or to
   * remove
   */
  void synchronizeTextures(lsw::glrs::RenderSystem& renderSystem) override;

  bool isMaterialDefined(const std::string& materialName);

  // void setDefaultMaterial(std::shared_ptr<Material> material);
  void setDefaultMaterial(const std::string& name);

  std::shared_ptr<lsw::geo::Material> makeDefaultMaterial();

 private:
  std::shared_ptr<izz::SceneGraph> m_sceneGraph;
  std::shared_ptr<lsw::ResourceManager> m_resourceManager;
  std::unordered_map<std::string, std::unique_ptr<lsw::ufm::UniformBlockManager>> m_uniformBlockManagers;
  std::unordered_map<std::string, lsw::geo::Material> m_materials;

  /// @brief Fbx specific material names are mapped to canonical material names
  /// that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  /// materials instances are instanced from an existing material definition.
  /// similar to how classes and objects work.
  std::unordered_map<std::string, lsw::geo::Material> m_materialInstances;

  std::string m_defaultMaterial{""};

  void readMaterialInstances(nlohmann::json& json);
  void readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& json);

  // @see IRenderSubsystem
  void onRender(entt::entity entity);
};

}  // namespace gl
}  // namespace izz