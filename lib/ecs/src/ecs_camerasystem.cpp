//
// Created by jlemein on 16-11-20.
//
#include <ecs_camerasystem.h>
#include <ecs_camera.h>
#include <ecs_transform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

using namespace affx;

ecs::CameraSystem::CameraSystem(entt::registry &registry)
    : m_registry(registry) {}

void ecs::CameraSystem::init() {

}

void ecs::CameraSystem::update(float dt) {
  auto view = m_registry.view<ecs::Camera, ecs::Transform>();

//  for (auto& e : view) {
//    auto& camera = view.get<ecs::Camera>(e);
//    auto& transform = view.get<ecs::Transform>(e);
//
////    camera.perspective = glm::perspective(camera.fovy, camera.aspect, camera.zNear, camera.zFar);
//  }
}
