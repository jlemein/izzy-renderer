//
// Created by jlemein on 05-08-21.
//
#pragma once

#include <izz_resourcemanager.h>
#include <entt/entt.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include "izz_scenegraphentity.h"
#include "izzgl_material.h"
#include "izzgl_rendersystem.h"
#include "uniform_uniformblockmanager.h"

namespace lsw {

class ResourceManager;

}  // namespace lsw

namespace izz {
namespace gl {

class RenderSystem;

/**
 * The material system manages the material components in the scene.
 * It is responsible for reading the material definitions (json) file that list
 * the available materials.
 *
 *
 */
class MaterialSystem {
 public:
  MaterialSystem(entt::registry& registry, std::shared_ptr<lsw::ResourceManager> resourceManager);

  virtual ~MaterialSystem() = default;

  void addMaterialDescription(std::string name, izz::geo::MaterialDescription md);

  izz::geo::MaterialDescription& getMaterialDescription(const std::string& name);
//  const MaterialDescription& getMaterialDescription(const std::string& name) const;

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
  Material& createMaterial(const std::string& name);
  //  Material createMaterial(MaterialDescription& materialDescription);
  Material& getMaterialById(int id);


  /**
   * Every frame this method is called to let the material system update it's shader parameters.
   * @param time Global simulation time.
   * @param dt Delta time passed since previous frame.
   */
  void update(float time, float dt);

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
  void synchronizeTextures(RenderSystem& renderSystem);

  bool hasMaterialDescription(const std::string& materialName);

  void setDefaultMaterial(const std::string& name);

  izz::gl::Material& makeDefaultMaterial();

 private:

  izz::geo::MaterialDescription& resolveMaterialDescription(const std::string name);

  /**
   * Compiles the shaders referred to by the description, and fills the material with the program id.
   * @param material                [out] Reference to material that will be filled with program id (on success).
   * @param materialDescription     [in]  Description of the material, containing the paths to the shader files.
   * @throws std::runtime_error     If shader compilation fails.
   */
  void compileShader(Material& material, const izz::geo::MaterialDescription& materialDescription);

  void allocateBuffers(Material& material, const izz::geo::MaterialDescription& materialDescription);
  UniformBuffer createUniformBuffer(const izz::geo::UniformBufferDescription& bufferDescription, const Material& m);
  //  void allocateTextureBuffers(MaterialDescription& materialDescription, Material& material);
  //  void allocateUniformBuffers();

  entt::registry& m_registry;
  std::shared_ptr<lsw::ResourceManager> m_resourceManager;
  std::unordered_map<std::string, std::unique_ptr<izz::ufm::UniformBlockManager>> m_uniformBlockManagers;

  /// @brief Contains the material definitions that are loaded from file.
  std::unordered_map<std::string, izz::geo::MaterialDescription> m_materialDescriptions;

  /// materials instances are instanced from an existing material definition.
  /// similar to how classes and objects work.
//  std::unordered_map<std::string, MaterialDescription> m_materialInstances;

  /// @brief Fbx specific material names are mapped to canonical material names
  /// that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  /// @brief The materials that are created. Maps material id to material.
  std::unordered_map<int, Material> m_createdMaterials;

  std::string m_defaultMaterial{""};

  void readMaterialInstances(nlohmann::json& json);
  void readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& json);

  // @see IRenderSubsystem
  void onRender(entt::entity entity);
};

}  // namespace gl
}  // namespace izz