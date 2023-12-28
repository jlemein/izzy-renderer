//
// Created by jlemein on 06-11-20.
//
#pragma once

#include <stdint.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <geo_boundingbox.h>
#include <geo_material.h>
#include <boost/functional/hash.hpp>

namespace lsw {
namespace geo {

enum class PolygonMode { kTriangles, kQuads };

struct universal_hash {
  template <class T>
  std::size_t operator()(T&& arg) const {
    // note - boost::hash. A thousand times better.

    using hasher_type = boost::hash<std::decay_t<T>>;
    auto hasher = hasher_type();
    return hasher(arg);
  }
};

struct Vertex {
  glm::vec3 position {0,0,0};
  glm::vec3 normal {0, 0, 0};
  glm::vec2 uv {0, 0};
};

struct Mesh {
  struct Vertex {
    float x, y, z;
  };
  struct Normal {
    float x, y, z;
  };
  struct Uv {float x, y;};

  std::string name;
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<float> tangents;
  std::vector<float> bitangents;

  std::vector<uint32_t> indices;

  std::shared_ptr<Material> material{nullptr};

  // describes the data
  PolygonMode polygonMode{PolygonMode::kTriangles};
};

inline auto as_tuple(Mesh::Vertex const& v) {
  return std::tie(v.x, v.y, v.z);
}

inline bool operator==(Mesh::Vertex const& l, Mesh::Vertex const& r) {
  return as_tuple(l) == as_tuple(r);
}

inline bool operator<(Mesh::Vertex const& l, Mesh::Vertex const& r) {
  return as_tuple(l) < as_tuple(r);
}

inline std::size_t hash_value(Mesh::Vertex const& arg) {
  std::size_t seed = 0;
  boost::hash_combine(seed, as_tuple(arg));
  return seed;
}

}  // namespace geo
}  // namespace lsw
