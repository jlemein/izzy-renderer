//
// Created by jlemein on 29-11-20.
//
#include <ecs_transform.h>
#include <ecs_transformutil.h>
using namespace affx::ecs;

void TransformUtil::SetPosition(Transform &t, const glm::vec3 &e) {
  t.localTransform[3] = glm::vec4(e, 1.0f);
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

void TransformUtil::Translate(Transform &transform, const glm::vec3 &translate) {
  transform.localTransform[3] += glm::vec4(translate, 0.0f);
}

void TransformUtil::SetWorldPosition(Transform &transform, const glm::vec3 &position) {
  transform.localTransform[3] += glm::vec4(position, 0.0f);
}
