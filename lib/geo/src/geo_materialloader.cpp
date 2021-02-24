//
// Created by jlemein on 10-02-21.
//
#include <geo_materialloader.h>

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

using namespace affx::geo;

MaterialSystem::MaterialSystem(const std::string &path)
    : m_materialConfigUri{path} {}

void MaterialSystem::initialize() {
  // create an empty structure (null)
  std::ifstream input(m_materialConfigUri);
  if (input.fail()) {
    auto msg =
        fmt::format("MaterialSystem fails to initialize: cannot read from '{}'",
                    m_materialConfigUri);
    spdlog::error(msg);
    throw std::runtime_error(msg);
  }

  json j;
  input >> j;

  for (auto &material : j["materials"]) {
    auto name = material["name"].get<std::string>();
    m_materials[name] =
        Material{.name = material["material_name"].get<std::string>(),
                 .vertexShader = material["vertex_shader"],
                 .geometryShader = material["geometry_shader"],
                 .fragmentShader = material["fragment_shader"]};
  }

  if (m_materials.count(j["default_material"]) > 0) {
    m_defaultMaterial = j["default_material"];
  } else {
    spdlog::warn("Cannot set default material to {}",
                 j["default_material"].get<std::string>());
  }
}

std::unique_ptr<affx::res::IResource>
MaterialSystem::loadResource(const std::string &name) {
  if (m_materials.count(name) == 0 &&
      m_materials.count(m_defaultMaterial) == 0) {
    throw std::runtime_error("Cannot load material. Material does not exist "
                             "and no default material set");
  }
  auto material = m_materials.count(name) > 0
                      ? m_materials.at(name)
                      : m_materials.at(m_defaultMaterial);

  return std::make_unique<affx::res::Resource<geo::Material>>(100, material);
}

std::unique_ptr<affx::res::IResource> MaterialSystem::makeDefaultMaterial() {
  return loadResource(m_defaultMaterial);
}