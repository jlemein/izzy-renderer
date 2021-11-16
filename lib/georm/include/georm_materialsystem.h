//
// Created by jlemein on 05-08-21.
//

#ifndef RENDERER_GEORM_MATERIALSYSTEM_H
#define RENDERER_GEORM_MATERIALSYSTEM_H

#include <ecsg_scenegraphentity.h>
#include <geo_material.h>
#include <glrs_rendersystem.h>
#include <res_resource.h>
#include <res_resourcefactory.h>
#include <uniform_uniformblockmanager.h>
#include <entt/entt.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include "georm_resourcemanager.h"

namespace lsw {

namespace ecsg {
class SceneGraph;
}  // namespace ecsg
namespace georm {

class ResourceManager;

/**
 * The material system manages the material components in the scene.
 * It is responsible for reading the material definitions (json) file that list
 * the available materials.
 *
 * Every material described in the materials file should be mapped to a data
 * structure.
 */
class MaterialSystem : public glrs::IMaterialSystem {
 public:
  MaterialSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<georm::ResourceManager> resourceManager);

  virtual ~MaterialSystem() = default;

  void loadMaterialsFromFile(const std::filesystem::path& path);

  /**
   * @brief Loads the material associated with the specified material name.
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
  std::shared_ptr<geo::Material> createMaterial(const std::string& name);

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
  void synchronizeTextures(glrs::RenderSystem& renderSystem) override;

  bool isMaterialDefined(const std::string& materialName);

  // void setDefaultMaterial(std::shared_ptr<Material> material);
  void setDefaultMaterial(const std::string& name);

  std::shared_ptr<geo::Material> makeDefaultMaterial();

 private:
  std::shared_ptr<ecsg::SceneGraph> m_sceneGraph;
  std::shared_ptr<georm::ResourceManager> m_resourceManager;

  //  std::unordered_map<std::string, MaterialPtr> m_cachedMaterials;
  //  std::unordered_map<unsigned int, res::Resource<geo::Material>>
  //      m_registeredMaterials;

  std::unordered_map<std::string, std::unique_ptr<ufm::UniformBlockManager>> m_uniformBlockManagers;

  std::unordered_map<std::string, geo::Material> m_materials;

  /// @brief Fbx specific material names are mapped to canonical material names
  /// that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  std::string m_defaultMaterial{""};
  unsigned int m_nextId = {1U};

  void readMaterialMappings(nlohmann::json& json);
  void readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& json);

  // @see IRenderSubsystem
  void onRender(entt::entity entity);
};

}  // namespace georm
}  // namespace lsw

#endif  // RENDERER_GEORM_MATERIALSYSTEM_H
