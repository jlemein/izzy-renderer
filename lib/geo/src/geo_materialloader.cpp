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
    auto materialName = material["material_name"].get<std::string>();

    // pass shader info
    std::string vertexShader, geometryShader, fragmentShader;
    try {
      vertexShader = material["vertex_shader"].get<std::string>();
      geometryShader = material["geometry_shader"].get<std::string>();
      fragmentShader = material["fragment_shader"].get<std::string>();
    } catch (nlohmann::detail::exception e) {
      throw std::runtime_error(
          fmt::format("Failed parsing shader names: {}", e.what()));
    }

    // load generic attributes
    glm::vec3 diffuse {0.0}, ambient {0.0}, specular{0.0};
    auto &properties = material["properties"];
    auto diffuseProperty = properties["diffuse_color"];
    if (diffuseProperty.size() >= 3) {
      diffuse = glm::vec3(diffuseProperty[0].get<float>(),
                          diffuseProperty[1].get<float>(),
                          diffuseProperty[2].get<float>());
    }
    auto specularProperty = properties["specular_color"];
    if (specularProperty.size() >= 3) {
      specular = glm::vec3(specularProperty[0].get<float>(),
                           specularProperty[1].get<float>(),
                           specularProperty[2].get<float>());
    }
    auto ambientProperty = properties["ambient_color"];
    if (ambientProperty.is_array()) {
      ambient = glm::vec3(ambientProperty[0].get<float>(),
                          ambientProperty[1].get<float>(),
                          ambientProperty[2].get<float>());
    }

    spdlog::info("Material {} with ambient color {}, {}, {} -- size {}", name, ambient.r, ambient.g, ambient.b, ambientProperty.count());

    m_materials[name] = Material{.name = materialName,
                                 .vertexShader = vertexShader,
                                 .geometryShader = geometryShader,
                                 .fragmentShader = fragmentShader,
                                 .diffuse = diffuse,
                                 .ambient = ambient,
                                 .specular = specular};
    spdlog::info("Loaded shader with name {}", vertexShader);
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