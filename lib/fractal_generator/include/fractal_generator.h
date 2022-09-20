//
// Created by jlemein on 15-11-20.
//
#pragma once

#include <geo_curve.h>

namespace izz {
namespace fractal {
class FractalGenerator {
public:
  FractalGenerator(unsigned int seed);

  izz::geo::Curve makeSierpinskiTriangle(unsigned int iterations);

private:
  unsigned int m_seed;
};

} // end package
} // end enterprise
