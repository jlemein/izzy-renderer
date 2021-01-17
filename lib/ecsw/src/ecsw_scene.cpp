//
// Created by jlemein on 13-01-21.
//
#include <ecsw_scene.h>
#include <fmt/format.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
using namespace artifax::ecsw;

Scene::Scene(const std::string& path) {
  // import scene file
  m_aiScene_p = m_importer.ReadFile(path, aiProcess_Triangulate);
  if (!m_aiScene_p) {
    throw std::runtime_error(fmt::format("Failed to load scene file: {}", path));
  }
}

inline const aiScene* Scene::get() {
  return m_aiScene_p;
}