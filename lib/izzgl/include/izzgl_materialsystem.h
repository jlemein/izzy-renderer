//
// Created by jlemein on 05-08-21.
//
#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
#include "izz_resourcemanager.h"
#include "izz_scenegraphentity.h"
#include "izzgl_irendercapabilityselector.h"
#include "izzgl_material.h"
#include "izzgl_mvp.h"
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

  /**
   * Registers a material template to the material system.
   * @param materialTemplate The material template.
   */
  void addMaterialTemplate(izz::geo::MaterialTemplate materialTemplate);

  /**
   * Registers a uniform buffer identified by a
   * @param name            The name of the uniform buffer.
   * @param uniformBuffer   The uniform buffer instance.
   */
  inline void registerUniformBuffer(std::string name, std::unique_ptr<IUniformBuffer> uniformBuffer) {
    m_uniformBuffers[name] = std::move(uniformBuffer);
  }

  /**
   * Creates a new material specified by the material template.
   * @param materialTemplate The material template.
   * @return a reference to a newly created material.
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

  /**
   * Retrieves a material from the materials in the scene, matching the specified material id.
   * @param materialId The material id.
   * @returns a material specified by the material id.
   *
   * @throws std::runtime_error if no material can be found with the specified material id.
   */
  Material& getMaterialById(int materialId);

  /**
   * Every frame this method is called to let the material system update it's shader parameters.
   * @param time Global simulation time.
   * @param dt Delta time passed since previous frame.
   */
  void update(float dt, float time);

  /**
   * Checks if a material template name is known to the material system.
   * @param materialTemplateName   The name of the template.
   * @return true if the material system has a template specification for the specified material template name. False otherwise.
   */
  bool hasMaterialTemplate(const std::string& materialTemplateName);

  /**
   * Sets the default material that will be used when creating a material.
   * The default material is used when no material template definition can be found (i.e. when @see \hasMaterialTemplate returns false).
   * @param name Material template name of the default material.
   */
  void setDefaultMaterial(const std::string& name);

  /**
   * Creates a material using the default material name.
   * @return A reference to a newly created material, owned by the material system.
   */
  izz::gl::Material& makeDefaultMaterial();

  const std::unordered_map<int, Material>& getCreatedMaterials();

  void setCapabilitySelector(const IRenderCapabilitySelector* selector);

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

  const geo::MaterialTemplate& getMaterialTemplateFromMaterial(int materialId) const;

  void updateUniformsForEntity(entt::entity e, Material& material);


 private:
  /**
   * Compiles the shaders referred to by the description, and fills the material with the program id.
   * @param materialTemplate       [in]  Description of the material, containing the paths to the shader files.
   *
   * @throws std::runtime_error     If shader compilation fails.
   */
  ShaderVariantInfo compileShader(const izz::geo::MaterialTemplate& materialTemplate);

  void allocateBuffers(Material& material, const izz::geo::MaterialTemplate& materialTemplate);

  void allocateTextureBuffers(Material& material, const std::unordered_map<std::string, izz::geo::TextureDescription>& textureDescriptions);

  /**
   * Creates a uniform buffer, using the uniform buffer description for the specified material.
   * @param bufferDescription Description of the uniform buffer (@see \izz::geo::UniformBufferDescription).
   * @param material material.
   * @return A pure uniform buffer data object. This buffer is used to send to the GPU.
   */
  UniformBuffer createUniformBuffer(const izz::geo::UniformBufferDescription& bufferDescription, const Material& material);

  entt::registry& m_registry;
  std::shared_ptr<izz::ResourceManager> m_resourceManager;
  std::string m_defaultMaterialTemplateName{""};
  const IRenderCapabilitySelector* m_capabilitySelector {nullptr};

  /// the registered uniform buffers
  std::unordered_map<std::string, std::unique_ptr<IUniformBuffer>> m_uniformBuffers;

  /// actual buffers in use, which is a subset of @see \m_uniformBuffers.
  std::unordered_set<std::string> m_uniformBuffersInUse{};

  /// @brief Contains the material definitions that are loaded from file.
  std::unordered_map<std::string, izz::geo::MaterialTemplate> m_materialTemplates;

  /// @brief Fbx specific material names are mapped to canonical material names that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  /// @brief The materials that have been created. Maps material id to material.
  std::unordered_map<int, Material> m_createdMaterials;
  std::unordered_map<std::string, int> m_createdMaterialNames;

  /// @brief maps shader program id to a shader variant information struct.
  std::unordered_map<int, ShaderVariantInfo> m_compiledShaderPrograms;

  /// @brief Maps template name to shader variant (program id).
  std::unordered_map<std::string, int> m_compiledMaterialTemplates;
};

}  // namespace gl
}  // namespace izz