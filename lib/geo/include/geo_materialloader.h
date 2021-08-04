//
// Created by jlemein on 10-02-21.
//

#ifndef RENDERER_GEO_MATERIALLOADER_H
#define RENDERER_GEO_MATERIALLOADER_H

#include <res_resourcefactory.h>
#include <geo_mesh.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace lsw {
namespace geo {

/**
 * The material system manages the materials in the scene.
 * It is responsible for reading the material definitions (json) file that list
 * the available materials.
 *
 * Every material described in the materials file should be mapped to a data
 * structure.
 */
class MaterialSystem : public lsw::res::ResourceFactory {
public:
  MaterialSystem(const std::string& path);

  void initialize();

  /// @brief Loads the material associated with the specified material name.
  /// @details the material is loaded based on material name. There are four possibilities:
  ///  1. The provided material name is mapped to a different material name (based on material mapping).
  ///  2. If the provided material name is not a key in the material map, a direct material lookup is performed.
  ///  3. If not found, then the default material is used.
  ///  4. If there is no default material, an exception is thrown.
  /// @returns a unique ptr
  std::unique_ptr<res::IResource> createResource(const std::string &name) override;

  bool isMaterialDefined(const std::string& materialName);

  //void setDefaultMaterial(std::shared_ptr<Material> material);
  void setDefaultMaterial(const std::string& name);

  std::unique_ptr<res::IResource> makeDefaultMaterial();

private:
  std::string m_materialConfigUri;
  std::unordered_map<unsigned int, res::Resource<geo::Material>> m_registeredMaterials;

  std::unordered_map<std::string, geo::Material> m_materials;

  /// @brief Fbx specific material names are mapped to canonical material names
  /// that are defined in a material definition.
  std::unordered_map<std::string, std::string> m_materialMappings;

  std::string m_defaultMaterial {""};
  unsigned int m_nextId = {1U};

  void readMaterialMappings(nlohmann::json& json);
  void readMaterialDefinitions(nlohmann::json& json);
};

} // end of package
} // end of enterprise

#endif // RENDERER_GEO_MATERIALLOADER_H
