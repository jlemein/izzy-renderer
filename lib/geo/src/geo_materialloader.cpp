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
    Material m;

    auto name = material["name"].get<std::string>();
    m.name = material["material_name"].get<std::string>();

    // pass shader info
    try {
      m.vertexShader = material["vertex_shader"].get<std::string>();
      m.geometryShader = material["geometry_shader"].get<std::string>();
      m.fragmentShader = material["fragment_shader"].get<std::string>();
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(
          fmt::format("Failed parsing shader names: {}", e.what()));
    }

    try {
      // load generic attributes
      auto &properties = material["properties"];
      auto diffuse = properties["diffuse_color"];
      if (diffuse.size() >= 3) {
        m.diffuse = glm::vec3(diffuse[0].get<float>(),
                              diffuse[1].get<float>(),
                              diffuse[2].get<float>());
        m.hasDiffuse = true;
      }
      auto specular = properties["specular_color"];
      if (specular.size() >= 3) {
        m.specular = glm::vec3(specular[0].get<float>(),
                               specular[1].get<float>(),
                               specular[2].get<float>());
        m.hasSpecular = true;
      }
      auto ambient = properties["ambient_color"];
      if (ambient.size() >= 3) {
        m.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
        m.hasAmbient = true;
      }

      spdlog::info("Material {} with ambient color {}, {}, {}", name, m.ambient.r, m.ambient.g, m.ambient.b);
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(
          fmt::format("Failed reading standard material color (diffuse, specular or ambient): {}", e.what()));
    }

    m_materials[name] = m;
    spdlog::info("Loaded material with name {} mapped to {}", name, m.name);
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