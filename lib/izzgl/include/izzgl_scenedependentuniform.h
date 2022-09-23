//
// Created by jeffrey on 28-9-22.
//
#pragma once

#include <entt/entt.hpp>

namespace izz::gl {

struct Material;

/// @brief opaque pointer to the uniform data structure.
using UniformBufferPtr = void*;

/**
 * Interface to a uniform buffer. This abstraction abstract a concrete uniform buffer.
 *
 * Examples of scene dependent uniforms are "model-view-projection" matrices, or lighting data.
 */
class IUniformBuffer {
 public:
  /// @brief Destructor
  virtual ~IUniformBuffer() = default;

  /**
   * Allocates and returns the uniform buffer data structure.
   *
   * @param t [out] Size argument. This reference will contain the size of the uniform buffer data structure in bytes.
   * @return A pointer to the uniform buffer data structure.
   */
  virtual UniformBufferPtr allocate(size_t& t) = 0;

  /**
   * Deallocates the uniform buffer data structure that was created by a earlier call to allocate.
   * @param bufferDataPtr Pointer to an existing uniform buffer data structure.
   */
  virtual void destroy(UniformBufferPtr bufferDataPtr) = 0;

  /**
   * Initializes the uniform data. This method is called for the lifetime of a uniform buffer.
   * This method can be used to initialize the buffer data.
   */
  virtual void onInit() {}

  /**
   * Called on frame start to offer the option to do a once per frame update.
   * This method is called once per frame for each used IUniformBuffer instance in the scene.
   * Even if there are multiple entities in the scene rendered using the same material, this method is called once.
   * @param dt      Delta time with previous frame (in seconds).
   * @param time    Total time running since application start (in seconds).
   */
  virtual void onFrameStart(float dt, float time) = 0;

  /**
   * Update the uniform buffer data structure with material properties. This is the conventional place
   * to synchronize material properties with the data structure that get's send to the shader program.
   * @param bufferDataPtr   Uniform buffer data structure.
   * @param material        Material.
   * @param dt              Delta time with previous frame (in seconds).
   * @param time            Total time since start of application (in seconds).
   */
  virtual void onUpdate(UniformBufferPtr bufferDataPtr, const gl::Material& material, float dt, float time) = 0;

  /**
   * Updates the uniform buffer data structure. Similar to @see \onUpdate, except this offers the flexibility
   * to adjust data per entity. This method is called just before rendering the entity, so it can be used to
   * set data that differs per entity. Example is the model matrix in the model-view-projection matrix.
   * @param e           Entity to be rendered.
   * @param material    Material used to render.
   */
  virtual void onEntityUpdate(entt::entity e, Material& material) = 0;
};

}  // namespace izz::gl
