//
// Created by jeffrey on 19-01-22.
//

#pragma once

#include <entt/fwd.hpp>

namespace izz {
namespace gl {

class DeferredRenderer {
 public:
  void render(const entt::registry& registry);

 private:
  int m_fbo {0};
};

}  // namespace gl
}  // namespace izz
