//
// Created by jeffrey on 24-4-22.
//
#pragma once

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <filesystem>

namespace izz {
namespace gl {

class MaterialSystem;

/**
 * @brief Reads in materials from JSON file and defines them in the material system.
 *
 * @details The JSON file offers two ways to define materials:
 * * Material definitions: defines the paths to the shaders and the names and datatypes of the properties defined in the shaders.
 * * Material instances: inherits a material definitions and defines different default values for the shader properties. This provides a comofrtable way
 *  to re-use material definitions, without having to duplicate the entire definition.
 *
 * A default material may be defined as well by referring to the name. This name must be present in the list of material definitions or instances.
 *
 */
class MaterialReader {
 public:
  MaterialReader(std::shared_ptr<MaterialSystem> materialSystem);

  void readMaterials(std::filesystem::path path);

 private:
  void readMaterialDefinitions(const std::filesystem::path& parent_path, nlohmann::json& json);
  void readMaterialInstances(nlohmann::json& json);

  std::shared_ptr<MaterialSystem> m_materialSystem {nullptr};
};
}
}