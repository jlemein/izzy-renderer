//
// Created by jeffrey on 17-12-21.
//
#pragma once

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <geo_effect.h>

namespace izz {

class SceneGraphHelper;

namespace gl {

class MaterialSystem;

class EffectSystem {
 public:
  EffectSystem(izz::SceneGraphHelper& sceneGraph, MaterialSystem& materialSystem);

  void readEffectsFromFile(const std::filesystem::path& path);
  void readEffectsFromJson(const nlohmann::json& json);
  void readFramebuffers(const nlohmann::json& json);

  /***
   * Creates a blank effect file.
   * @param name The name of the effect file.
   * @return
   */
  geo::cEffect createEffect(const std::string& name);

  /**
   * Get an effect that has been loaded before by a call to readEffectsFromFile.
   * @param cEffect A effect component.
   * @return A reference to the underlying effect referenced by cEffect.
   */
  geo::Effect& getEffect(geo::cEffect cEffect);

  /***
   * @overload
   */
  const geo::Effect& getEffect(geo::cEffect cEffect) const;

  /// @brief Initializes the effects and framebuffer configurations.
  void initialize();

 private:
  SceneGraphHelper& m_sceneGraph;
  MaterialSystem& m_materialSystem;

  std::unordered_map<std::string, geo::Effect> m_effects;
  std::unordered_map<std::string, geo::FramebufferConfiguration> m_framebuffers;
};

}
}
