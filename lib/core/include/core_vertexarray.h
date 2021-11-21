//
// Created by jlemein on 28-11-20.
//
#pragma once

/**
 * @brief Efficient vertex storage structure.
 */
class VertexArray {
 public:
  VertexArray(int capacity);

  void addVertex(float x, float y, float z);
  void addVertex(const glm::vec3& p);

};
