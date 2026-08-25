#pragma once

#include <izzgl_material.h>
#include <izzgl_scenedependentuniform.h>
#include "izz_skybox.h"
#include <ecs_camera.h>
#include <ecs_transform.h>

namespace izz {
namespace ufm {

/**
 * CPU based uniform data that maps the uniform data with the GLSL shader.
 */
struct ModelViewProjection {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec3 viewPos;

  /// name of the uniform buffer in the shader object.
  static inline const char* BUFFER_NAME = "ModelViewProjection";
};

struct MvpShared {
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
};

class MvpSharedUniformUpdater : public izz::gl::IUniformBuffer {
 public:
  MvpSharedUniformUpdater(entt::registry& registry)
    : m_registry{registry} {}

  virtual ~MvpSharedUniformUpdater() = default;

  virtual izz::gl::UniformBufferPtr allocate(size_t& t) override {
    t = sizeof(ModelViewProjection);
    return new ModelViewProjection;
  }

  virtual void destroy(izz::gl::UniformBufferPtr data) override {
    auto mvp = reinterpret_cast<ModelViewProjection*>(data);
    delete mvp;
  }

  virtual void onUpdate(void* data, const gl::Material& m, float dt, float time) override {
    // do nothing
  }

  /**
   * Precomputes the view-projection matrix of the model-view-projection matrix.
   * This happens once per frame, since the projection and view is the same for all.
   * @param dt delta time with previous frame (in seconds).
   * @param time total time running (in seconds).
   */
  virtual void onFrameStart(float dt, float time) override {
    auto cameraEntity = m_registry.view<ecs::Camera>()[0];
    if (cameraEntity == entt::null) {
      throw std::runtime_error("No camera specified. Cannot update MVP's.");
    }

    auto& cameraTransform = m_registry.get<izz::ecs::Transform>(cameraEntity);
    m_mvpShared.viewMatrix = glm::inverse(cameraTransform.worldTransform);

    auto& activeCamera = m_registry.get<izz::ecs::Camera>(cameraEntity);
    m_mvpShared.projectionMatrix = glm::perspective(activeCamera.fovx, activeCamera.aspect, activeCamera.zNear, activeCamera.zFar);
  }

  virtual void onEntityUpdate(entt::entity e, izz::gl::Material& material) override {
    auto transform = m_registry.try_get<izz::ecs::Transform>(e);
    auto model = transform != nullptr ? transform->worldTransform : glm::mat4(1.0F);

    auto uniformBuffer = material.uniformBuffers.at(izz::ufm::ModelViewProjection::BUFFER_NAME);
    ModelViewProjection* mvp = reinterpret_cast<ModelViewProjection*>(uniformBuffer.data);
    mvp->model = model;
    mvp->view = m_mvpShared.viewMatrix;
    mvp->projection = m_mvpShared.projectionMatrix;

    // camera position is stored in 4-th column of inverse view matrix.
    mvp->viewPos = glm::inverse(m_mvpShared.viewMatrix)[3];
  }

 private:
  entt::registry& m_registry;
  MvpShared m_mvpShared;
};

}  // namespace ufm
}  // namespace izz