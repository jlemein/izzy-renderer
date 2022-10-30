//
// Created by jlemein on 01-12-20.
//
#pragma once

#include <glm/glm.hpp>
#include <ostream>

namespace izz {

struct BoundingBox {
  glm::vec3 min{.0F};
  glm::vec3 max{.0F};

  glm::vec3 getSize() {
    return max - min;
  }

  glm::vec3 getCenter() {
    return min + 0.5F * getSize();
  }

  BoundingBox operator+(const BoundingBox& other) {
    return BoundingBox{.min = glm::min(min, other.min), .max = glm::max(max, other.max)};
  }

  BoundingBox operator-(const BoundingBox& other) {
    return BoundingBox{.min = glm::max(min, other.min), .max = glm::min(max, other.max)};
  }

  BoundingBox& operator+=(const BoundingBox& other) {
    this->min = glm::min(min, other.min);
    this->max = glm::max(max, other.max);
    return *this;
  }

  BoundingBox& operator-=(const BoundingBox& other) {
    this->min = glm::min(max, other.max);
    this->max = glm::max(min, other.min);
    return *this;
  }
};

}  // namespace izz
