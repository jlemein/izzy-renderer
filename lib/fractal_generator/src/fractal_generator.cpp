//
// Created by jlemein on 15-11-20.
//
#include <fractal_generator.h>
#include <fractal_treefactory.h>
using namespace izz;
using namespace izz::fractal;

FractalGenerator::FractalGenerator(unsigned int seed) : m_seed(seed) {}

geo::Curve FractalGenerator::makeSierpinskiTriangle(unsigned int iterations) {
  geo::Curve curve;
  curve.width = 1.0F;

  auto tree = FractalTreeFactory::makeSierpinskiTriangle(iterations);
  tree->draw();
  const auto &state = tree->getState();

  curve.vertices.reserve(state.positions.size());
  for (int i = 0; i < state.positions.size(); ++i) {
    curve.vertices.push_back(state.positions[i]);
  }

  return curve;
}