//
// Created by jlemein on 14-11-20.
//

#ifndef GLVIEWER_FRACTAL_PENROSETILING_H
#define GLVIEWER_FRACTAL_PENROSETILING_H

#include <glm/glm.hpp>
#include <list>
#include <math.h>
#include <vector>
struct PenroseTile {};

// P1 tiles
struct Pentagram {
  float centerX {0.0F}, centerY {0.0F};
  float radius {1.0F};
  float orientation {0.0F};

  float sideLength() {
    return radius * std::cos(glm::radians(54.0F));
  }

  float height() {
    float s2 = sideLength()/2.0F;
    return s2 * std::tan(54.0F);
  }

  float starHeight() {
    return std::sin(glm::radians(72.0F))*sideLength();
  }

  float shrinkRatio() {
    auto top = 2.0F * (std::tan(glm::radians(54.0F))+std::sin(glm::radians(72.0F)));
    return top / std::tan(glm::radians(54.0F));
  }

};
struct Boat {};
struct Diamond {};

// P2 tiles
struct Dart{};
struct Kite{};


class PenroseTiling {
private:

public:
  PenroseTiling(float r) {
    m_tiles.emplace_back(Pentagram{0,0, r, 0.0F});
  }

  int state = 1;

  void decompose() {
    auto sizeOfVector = m_tiles.size();

    for (auto i =0; i<sizeOfVector; ++i) {
      Pentagram& pentagram = m_tiles[i];

      Pentagram p;
      p.centerX = pentagram.centerX;
      p.centerY = pentagram.centerY;
      p.orientation = pentagram.orientation + glm::radians(180.0F);
      p.radius = pentagram.radius / pentagram.shrinkRatio();

      m_tiles.emplace_back(p);
    }
    std::cout << "Number of tiles after 1 decompose: " << m_tiles.size() << std::endl;
  }

  std::vector<Pentagram> m_tiles;

  void addVertex(std::vector<float>& verts, const glm::vec3& p){
    verts.push_back(p.x);
    verts.push_back(p.y);
    verts.push_back(p.z);
  }

  void fillVertices(std::vector<float>& verts) {

    for (auto& tile : m_tiles) {
      auto zero = glm::vec3(tile.centerX + std::sin(tile.orientation) * tile.radius,
                tile.centerY + std::cos(tile.orientation) * tile.radius, 0.0F);

      addVertex(verts, zero);

      for (int i = 1; i <= 4; ++i) {
        float radians = tile.orientation + i * glm::radians(360.0 / 5.0F);
        std::cout << "Radians at " << i << " = " << glm::degrees(radians)
                  << std::endl;

        auto p = glm::vec3(tile.centerX + std::sin(radians) * tile.radius,
                           tile.centerY + std::cos(radians) * tile.radius, 0.0F);
        addVertex(verts, p);
        addVertex(verts, p);
      }
      addVertex(verts, zero);
    }
  }

};

#endif // GLVIEWER_FRACTAL_PENROSETILING_H
