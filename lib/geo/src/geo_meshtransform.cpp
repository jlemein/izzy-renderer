//
// Created by jlemein on 10-11-20.
//
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_meshtransform.h>
#include <geo_shapeutil.h>
#include <glm/gtx/quaternion.hpp>

using namespace affx;
using namespace affx::geo;

void MeshTransform::ScaleToUniformSize(Mesh &mesh, float size) {
  auto bb = ShapeUtil::computeBoundingBox(mesh);
  auto scaleX = bb.xMax - bb.xMin;
  auto scaleY = bb.yMax - bb.yMin;
  auto scaleZ = bb.zMax - bb.zMin;

  auto maxSize = std::max(std::max(scaleX, scaleY), scaleZ);
  auto correctionScale = size / maxSize;
//  std::cout << "Correction scale: " << correctionScale << " size: " << size << " maxsize: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
//  std::cout << "bbox: " << "[" << bb.xMin << " x " << bb.xMax << " ], [" << bb.yMin << " x " << bb.yMax << " ], [" << bb.zMin << " x " << bb.zMax << " ]\n";

  // make sure model is on ground plane
  for (unsigned int i=0; i<mesh.vertices.size(); i+=3) {
    auto& vx = mesh.vertices[i];
    auto& vy = mesh.vertices[i+1];
    auto& vz = mesh.vertices[i+2];

//    vy -= bb.yMin;

    vx *= correctionScale;
    vy *= correctionScale;
    vz *= correctionScale;
  }
}

void MeshTransform::ScaleToUniformSize(Curve &curve, float size) {
  auto bb = ShapeUtil::computeBoundingBox(curve);
  auto scaleX = bb.xMax - bb.xMin;
  auto scaleY = bb.yMax - bb.yMin;
  auto scaleZ = bb.zMax - bb.zMin;

  auto maxSize = std::max(std::max(scaleX, scaleY), scaleZ);
  auto correctionScale = size / maxSize;

  // make sure model is on ground plane
  for (unsigned int i=0; i<curve.vertices.size(); i+=3) {
    auto& vx = curve.vertices[i];
    auto& vy = curve.vertices[i+1];
    auto& vz = curve.vertices[i+2];

    vy -= bb.yMin;

    vx *= correctionScale;
    vy *= correctionScale;
    vz *= correctionScale;
  }
}

void MeshTransform::CenterMeshToWorldOrigin(Mesh &mesh) {
//
}

void MeshTransform::Translate(Mesh& mesh, const glm::vec3& translation) {
  // compare i+2 because we always need to be able to process multiples of 3 vertices.
  for (unsigned int i=0U; i+2 < mesh.vertices.size(); i+=3) {
    mesh.vertices[i] += translation.x;
    mesh.vertices[i+1] += translation.y;
    mesh.vertices[i+2] += translation.z;
  }
}

void MeshTransform::Rotate(Mesh& mesh, const glm::quat& q) {
  for (unsigned int i=0U; i+2 < mesh.vertices.size(); i+=3) {
    glm::vec4 transformed = glm::toMat4(q) * glm::vec4(mesh.vertices[i], mesh.vertices[i+1], mesh.vertices[i+2], 1.0F);
    mesh.vertices[i] = transformed.x;
    mesh.vertices[i+1] = transformed.y;
    mesh.vertices[i+2] = transformed.z;
  }
}

void MeshTransform::RotateX(Mesh& mesh, float degrees) {
  return Rotate(mesh, glm::angleAxis(glm::radians(degrees), glm::vec3(1.0F, .0F, .0F)));
}

void MeshTransform::RotateY(Mesh& mesh, float degrees) {
  return Rotate(mesh, glm::angleAxis(glm::radians(degrees), glm::vec3(.0F, 1.0F, .0F)));
}

void MeshTransform::RotateZ(Mesh& mesh, float degrees) {
  return Rotate(mesh, glm::angleAxis(glm::radians(degrees), glm::vec3(.0F, .0F, 1.0F)));
}
