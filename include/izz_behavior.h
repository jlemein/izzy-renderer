//
// Created by jlemein on 30-11-20.
//
#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <functional>

namespace izz {
/**
 * @brief Behavior allows to add custom logic to th
 */
struct Behavior {
  entt::entity subject;
  entt::entity target;
  std::function<void(izz::Izzy& izzy, entt::entity subject, entt::entity target)> function;
};

}  // namespace izz
