//
// Created by jlemein on 17-11-20.
//
#include <ecs_camera.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_firstpersoncontrolsystem.h>
#include <ecs_moveable.h>
#include <ecs_transform.h>

#include <io_inputsystem.h>
#include <io_keyconstants.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <geo_mesh.h>
#include <iostream>

using namespace lsw;

namespace
{
const glm::vec3 kForwardVector = glm::vec3(0.0F, 0.0F, 1.0F);

void
print(const char *str, const glm::mat4 &m)
{
  std::cout << str << std::endl;
  for (int i = 0; i < 16; ++i) {
    int c = i % 4;
    int r = i / 4;
    std::cout << m[r][c] << " ";
    if (c == 3)
      std::cout << std::endl;
  }
}

void
print(const char *str, const glm::vec4 &v)
{
  std::cout << str << std::endl;
  for (int i = 0; i < 4; ++i) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}
}  // namespace

ecs::FirstPersonMovementSystem::FirstPersonMovementSystem(entt::registry &registry,
                                                          io::InputSystem *inputSystem)
    : m_registry(registry), m_inputSystem(inputSystem)
{
}

void
ecs::FirstPersonMovementSystem::init()
{
//  auto view = m_registry.view<ecs::Transform, ecs::FirstPersonControl>();
//  for (auto e : view) {
//    auto &t = m_registry.get<Transform>(e);
//    auto &fpc = m_registry.get<FirstPersonControl>(e);
//
//    fpc.rotationX = 85.0F;
//    fpc.rotationY = 0.0F;
//  }
}

void ecs::FirstPersonMovementSystem::getRelativeMovement(float& forward, float& right)
{
  if (m_inputSystem->isKeyPressed('W')) {  // move forward (z axis)
    forward += 1.0F;
  }
  if (m_inputSystem->isKeyPressed('S')) {  // move backward (z axis)
    forward -= 1.0F;
  }
  if (m_inputSystem->isKeyPressed('A')) {  // move left (z axis)
    right -= 1.0F;
  }
  if (m_inputSystem->isKeyPressed('D')) {  // move right (z axis)
    right += 1.0F;
  }
}

void
ecs::FirstPersonMovementSystem::update(float dt)
{
  // TODO make use of command pattern

  auto view = m_registry.view<ecs::Transform, ecs::FirstPersonControl>();
  float forward{0.0F};
  float right{0.0F};
  double mouseX, mouseY;

  for (auto e : view) {
    auto &transform = m_registry.get<Transform>(e);
    auto &fpActor = m_registry.get<FirstPersonControl>(e);

    getRelativeMovement(forward, right);
    m_inputSystem->getRelativeMouseMovement(&mouseX, &mouseY);



    fpActor.rotationX += static_cast<float>(mouseX * 45.0 * fpActor.sensitivityX);
    fpActor.rotationY += static_cast<float>(mouseY * 45.0 * fpActor.sensitivityY);

    if (fpActor.rotationX > 180.0F) {
      fpActor.rotationX -= 360.0F;
    } else if (fpActor.rotationX < 180.0F) {
      fpActor.rotationX += 360.0F;
    }

    if (fpActor.rotationY >= 90.0F) {
      fpActor.rotationY = 90.0F;
    } else if (fpActor.rotationY <= -90.0F) {
      fpActor.rotationY = -90.0F;
    }

    auto qy = glm::angleAxis(glm::radians(-fpActor.rotationY), glm::vec3(1.0F, 0.0F, 0.0F));
    qy = glm::angleAxis(glm::radians(-fpActor.rotationX), glm::vec3(0.0F, 1.0F, 0.0F)) * qy;
    qy = glm::normalize(qy);
    auto cameraOrientation = glm::toMat4(qy);

    // adjust position
    auto velocity = m_registry.all_of<Moveable>(e) ? m_registry.get<Moveable>(e).velocity
                                                : fpActor.defaultVelocity;
    if (m_inputSystem->isKeyPressed(io::Key::kLEFT_SHIFT)) {
      velocity *= 3.0F;
    }

    auto cameraPosition = glm::vec3(transform.localTransform[3]);

    glm::vec3 relativeMovement = cameraOrientation *
                                 glm::vec4(right * velocity * dt, 0.0F,
                                           -forward * velocity * dt, 1.0F);
    glm::mat4 translate = glm::translate(glm::mat4(1.0F), relativeMovement);

    cameraPosition += glm::vec3(translate[3]);

    transform.localTransform = glm::translate(glm::mat4(1.0f),
                                              glm::vec3(cameraPosition)) *
                               cameraOrientation;
  }
}