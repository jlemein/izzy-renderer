//
// Created by jlemein on 16-11-20.
//
#include <ecs_camera.h>
#include <ecs_camerasystem.h>
#include <ecs_transform.h>

#include <entt/entt.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lsw;

ecs::CameraSystem::CameraSystem(entt::registry& registry)
  : m_registry(registry)
{}

void ecs::CameraSystem::init() {}

void ecs::CameraSystem::setFramebufferSize(int width, int height) {
  m_bufferWidth = width;
  m_bufferHeight = height;
}

void ecs::CameraSystem::update(float dt) {
  auto view = m_registry.view<ecs::Camera, ecs::Transform>();

  for (auto& e : view) {
    auto& camera = view.get<ecs::Camera>(e);
    camera.aspect = static_cast<float>(m_bufferWidth) / m_bufferHeight;
    auto& transform = view.get<ecs::Transform>(e);
    ////    camera.perspective = glm::perspective(camera.fovy, camera.aspect, camera.zNear, camera.zFar);
  }
}
