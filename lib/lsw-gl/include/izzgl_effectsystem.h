//
// Created by jeffrey on 17-12-21.
//
#pragma once

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <geo_effect.h>

namespace izz {

class SceneGraph;

namespace gl {

class MaterialSystem;

class EffectSystem {
 public:
  EffectSystem(izz::SceneGraph& sceneGraph, MaterialSystem& materialSystem);

  void readEffectsFromFile(const std::filesystem::path& path);
  void readEffectsFromJson(const nlohmann::json& json);
  void readFramebuffers(const nlohmann::json& json);

  /// @brief Initializes the effects and framebuffer configurations.
  void initialize();

 private:
  SceneGraph& m_sceneGraph;
  MaterialSystem& m_materialSystem;

  std::unordered_map<std::string, geo::Effect> m_effects;
  std::unordered_map<std::string, geo::FramebufferConfiguration> m_framebuffers;
};

}
}
