//
// Created by jeffrey on 18-02-22.
//
#pragma once

#include <entt/fwd.hpp>

class RenderableComponentFactory {
 public:
  virtual void addRenderableComponent(entt::registry& registry, entt::entity e, int materialId) = 0;
};
