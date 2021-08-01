//
// Created by jlemein on 15-11-20.
//

#ifndef GLVIEWER_FRACTAL_GENERATOR_H
#define GLVIEWER_FRACTAL_GENERATOR_H

#include <geo_curve.h>
#include <geo_mesh.h>

namespace lsw {
namespace fractal {
class FractalGenerator {
public:
  FractalGenerator(unsigned int seed);

  lsw::geo::Curve makeSierpinskiTriangle(unsigned int iterations);

private:
  unsigned int m_seed;
};

} // end package
} // end enterprise

#endif // GLVIEWER_FRACTAL_GENERATOR_H
