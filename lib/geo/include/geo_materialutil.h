//
// Created by jlemein on 01-12-21.
//
#pragma once

namespace lsw {
namespace geo {

struct Material;

/**
 * @brief The material util provides functions that deal with with materials.
 * In this case the util is more a factory class.
 *
 * An important design decision is that all uniform properties are stored in uniform blocks.
 * Some of those uniform blocks are shared between the same type of materials, or even between
 * different materials. Think about the ModelViewProjection matrices between different materials.
 * Uniform blocks appear in two variants:
 * - Internal uniform parameters: material properties for the material. They are adjusted per material, such as the color, the shininess, etc.
 * - External uniform parameters: they are under control of the material system. The user should not need to know about these properties.
 *
 * Dealing with materials can be tricky. The following situations may be wanted:
 * - Unique materials: Changing the properties does not affect the appearance of other materials using the same shaders.
 * - Shared materials: Changing the properties will affect the appearance of all instances using this material.
 */
class MaterialUtil {
 public:
  /**
   * Clones the material and creates a unique material with independent **user properties**.
   * Note that external properties might still be shared between materials.
   *
   * @param [in] material    Const reference to material that will be copied.
   */
  static geo::Material CloneMaterial(const geo::Material& material);
};

}  // namespace geo
}  // namespace lsw