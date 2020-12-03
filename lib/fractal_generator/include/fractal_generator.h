//
// Created by jlemein on 15-11-20.
//

#ifndef GLVIEWER_FRACTAL_GENERATOR_H
#define GLVIEWER_FRACTAL_GENERATOR_H

#include <shp_mesh.h>
#include <shp_curve.h>

namespace artifax {
namespace fractal {
class FractalGenerator {
public:
  FractalGenerator(unsigned int seed);

  artifax::shp::Curve makeSierpinskiTriangle(unsigned int iterations);

private:
  unsigned int m_seed;
};

} // end package
} // end enterprise

#endif // GLVIEWER_FRACTAL_GENERATOR_H
