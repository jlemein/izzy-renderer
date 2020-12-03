//
// Created by jlemein on 10-11-20.
//
#include <shp_meshtransform.h>
#include <shp_mesh.h>
#include <shp_curve.h>
using namespace artifax;
using namespace artifax::shp;

void MeshTransform::scaleToUniformSize(Mesh &mesh, float size) {
  const auto& bb = mesh.getBoundingBox();
  auto scaleX = bb.xMax - bb.xMin;
  auto scaleY = bb.yMax - bb.yMin;
  auto scaleZ = bb.zMax - bb.zMin;

  auto maxSize = std::max(std::max(scaleX, scaleY), scaleZ);
  auto correctionScale = size / maxSize;
  std::cout << "Correction scale: " << correctionScale << " size: " << size << " maxsize: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
  std::cout << "bbox: " << "[" << bb.xMin << " x " << bb.xMax << " ], [" << bb.yMin << " x " << bb.yMax << " ], [" << bb.zMin << " x " << bb.zMax << " ]\n";

  // make sure model is on ground plane
  for (unsigned int i=0; i<mesh.vertices.size(); i+=3) {
    auto& vx = mesh.vertices[i];
    auto& vy = mesh.vertices[i+1];
    auto& vz = mesh.vertices[i+2];

    vy -= bb.yMin;

    vx *= correctionScale;
    vy *= correctionScale;
    vz *= correctionScale;
  }
}

void MeshTransform::scaleToUniformSize(Curve &curve, float size) {
  const auto& bb = curve.getBoundingBox();
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

void MeshTransform::centerMeshToWorldOrigin(Mesh &mesh) {
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

void MeshTransform::Rotate(Mesh& mesh, const glm::quat& quaternion) {
//
}