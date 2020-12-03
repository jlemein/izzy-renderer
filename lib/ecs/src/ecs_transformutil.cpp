//
// Created by jlemein on 29-11-20.
//
#include <ecs_transformutil.h>
#include <ecs_transform.h>
using namespace artifax::ecs;

void TransformUtil::SetPosition(Transform& t, const glm::vec3& e) {
  t.localTransform[3] = glm::vec4(e, 1.0f);
}
