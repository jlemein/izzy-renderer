//
// Created by jlemein on 01-12-20.
//
#pragma once

#include <entt/entity/registry.hpp>
#include "geo_curve.h"
#include "geo_mesh.h"
#include "gl_renderable.h"
#include "izz_debug.h"

#include <unordered_map>
#include "izz_debugmodel.h"

namespace izz {

class Izzy;

/**!
 * @brief Responsible for showing debug shapes for renderable objects
 */
class DebugSystem {
 public:
  DebugSystem(izz::Izzy& izzy, entt::registry& registry);

  void init();

  void update(float dt);

 private:
  izz::Izzy& m_izzy;
  entt::registry& m_registry;
};

}  // namespace izz
