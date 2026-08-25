//
// Created by jeffrey on 12-10-22.
//
#pragma once

#include "izz_capabilities.h"

namespace izz {

class MaterialTemplate;

namespace gl {

using RenderCapabilities = Capabilities;

/**
 * Abstract interface for a feature selector. A capability selector decides which features to
 * enable or disable for a material. For example, this could depend on many different factors such
 * as hardware capabilities, but also render system strategies. For example: rendering shadows or
 * transparency can be done in many different ways. The only one deciding which one to choose is the
 * material feature selector. Usually the render system controls the material feature selector.
 */
class IRenderCapabilitySelector {
 public:
  virtual ~IRenderCapabilitySelector() = default;

  /**
   * Decides which features to enable in the material shader.
   * For example, a render system implements it's own render strategy. It decides which materials
   * to use
   * @param materialTemplate
   */
  virtual RenderCapabilities selectRenderCapabilities(const izz::MaterialTemplate& materialTemplate) const = 0;
};

}  // namespace gl
}  // namespace izz
