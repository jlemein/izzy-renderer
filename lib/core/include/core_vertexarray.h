//
// Created by jlemein on 28-11-20.
//

#ifndef GLVIEWER_CORE_VERTEXARRAY_H
#define GLVIEWER_CORE_VERTEXARRAY_H

/**
 * @brief Efficient vertex storage structure.
 */
class VertexArray {
 public:
  VertexArray(int capacity);

  void addVertex(float x, float y, float z);
  void addVertex(const glm::vec3& p);

};
#endif  // GLVIEWER_CORE_VERTEXARRAY_H
