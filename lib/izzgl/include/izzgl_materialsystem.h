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

namespace izz {
class ResourceManager;
namespace gl {

class RenderSystem;

/// @brief information about shader variant.
struct ShaderVariantInfo {
  /// Program id of the shader variant.
  unsigned int programId{0};

  /// Number of instances in the scene currently using this shader variant.
  int numberOfInstances{0};

  std::string materialTemplateName;
};

/**
 * The material system manages the material components in the scene.
 * It is responsible for reading the material definitions (json) file that list
 * the available materials.
 *
 *
 */
class MaterialSystem {
 public:
  static inline const char* ID = "MaterialSystem";

  MaterialSystem(entt::registry& registry, std::shared_ptr<izz::ResourceManager> resourceManager);

  virtual ~MaterialSystem() = default;

  void addMaterialTemplate(izz::geo::MaterialTemplate materialTemplate);

  /**
   * Creates a new material from template
   * @param materialTemplate
   * @return
   */
  Material& createMaterial(const izz::geo::MaterialTemplate& materialTemplate);

  /**
   * Creates a new material with it's own set of data.
   *
   * @param meshMaterialName    [in] Name of the material.
   * @param instanceName        [in] Optional. If specified and not empty, then the name should not collide with any material created before.
   *                            Otherwise it throws an std::runtime_error.
   * @return a reference to the created material.
   */
  Material& createMaterial(std::string meshMaterialName, std::string instanceName = "");

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

  bool hasMaterialTemplate(const std::string& materialTemplateName);

  void setDefaultMaterial(const std::string& name);

  izz::gl::Material& makeDefaultMaterial();

  const std::unordered_map<int, Material>& getCreatedMaterials();

  /**
   * @brief returns information about the shader variant corresponding to the specified material.
   * @param material [in]   Material to request shader variant info.
   * @return shader variant info struct.
   */
  const ShaderVariantInfo& getShaderVariantInfo(const Material& material) const;

  const std::unordered_map<int, ShaderVariantInfo>& getShaderPrograms() const;

  /**
   * Looks up the corresponding material template, corresponding to the material template name.
   * The name of the material should be present in the materials.json file.
   * @param materialTemplateName The name of a material template, or definition, which should be present in the materials.json file.
   * @returns a reference to the material template.
   */
  const geo::MaterialTemplate& getMaterialTemplate(std::string materialTemplateName);

 private:
  /**
   * Compiles the shaders referred to by the description, and fills the material with the program id.
   * @param materialTemplate     [in]  Description of the material, containing the paths to the shader files.
   * @throws std::runtime_error     If shader compilation fails.
   */
  ShaderVariantInfo compileShader(const izz::geo::MaterialTemplate& materialTemplate);

  void allocateBuffers(Material& material, const izz::geo::MaterialTemplate& materialTemplate);
  void allocateTextureBuffers(Material& material, const std::unordered_map<std::string, izz::geo::TextureDescription>& textureDescriptions);

  UniformBuffer createUniformBuffer(const izz::geo::UniformBufferDescription& bufferDescription, const Material& m);
  //  void allocateTextureBuffers(MaterialDescription& materialDescription, Material& material);
  //  void allocateUniformBuffers();

  entt::registry& m_registry;
  std::shared_ptr<izz::ResourceManager> m_resourceManager;
  std::unordered_map<std::string, std::unique_ptr<izz::ufm::UniformBlockManager>> m_uniformBlockManagers;

  /// @brief Contains the material definitions that are loaded from file.
  std::unordered_map<std::string, izz::geo::MaterialTemplate> m_materialTemplates;

  /// @brief Fbx specific material names are mapped to canonical material names
  /// that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  /// @brief The materials that have been created. Maps material id to material.
  std::unordered_map<int, Material> m_createdMaterials;
  std::unordered_map<std::string, int> m_createdMaterialNames;

  /// @brief maps shader program id to a shader variant information struct.
  std::unordered_map<int, ShaderVariantInfo> m_compiledShaderPrograms;

  /// @brief Maps template name to shader variant (program id).
  std::unordered_map<std::string, int> m_compiledMaterialTemplates;


  std::string m_defaultMaterialTemplateName{""};
};

}  // namespace gl
}  // namespace izz