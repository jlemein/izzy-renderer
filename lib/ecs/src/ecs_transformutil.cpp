//
// Created by jlemein on 29-11-20.
//
#include <ecs_transform.h>
#include <ecs_transformutil.h>
using namespace lsw::ecs;

void TransformUtil::SetPosition(Transform &t, const glm::vec3 &e) {
  t.localTransform[3] = glm::vec4(e, 1.0f);
}

void TransformUtil::Scale(glm::mat4& transform, float scale) {
    transform[0][0] *= scale;
    transform[1][1] *= scale;
    transform[2][2] *= scale;
}

glm::mat4 TransformUtil::Scale(const glm::mat4& transform, float scale) {
    glm::mat4 m = transform;
    TransformUtil::Scale(m, scale);
    return m;
}

void TransformUtil::Scale(Transform &transform, float scale) {
  transform.localTransform[0][0] *= scale;
  transform.localTransform[1][1] *= scale;
  transform.localTransform[2][2] *= scale;
}

void TransformUtil::Scale(Transform &transform, const glm::vec3 &scale) {
  transform.localTransform[0][0] *= scale.x;
  transform.localTransform[1][1] *= scale.y;
  transform.localTransform[2][2] *= scale.z;
}

void TransformUtil::RotateEuler(Transform &transform, const glm::vec3& axis, float radians) {
  transform.localTransform = glm::rotate(transform.localTransform, radians, axis);
}

glm::mat4 TransformUtil::RotateEulerRadians(const glm::vec3& axis, float radians) {
  return glm::rotate(glm::mat4(1.0), radians, axis);
}

glm::mat4 TransformUtil::RotateEulerDegrees(const glm::vec3& axis, float degrees) {
  return RotateEulerRadians(axis, static_cast<float>(M_PI * degrees/180.0));
}

void TransformUtil::Translate(Transform &transform, const glm::vec3& translate) {
  transform.localTransform[3] += glm::vec4(translate, 0.0f);
}

void TransformUtil::SetWorldPosition(Transform &transform, const glm::vec3 &position) {
  transform.localTransform[3] += glm::vec4(position, 0.0f);
}
