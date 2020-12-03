//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_FRACTAL_TREEFACTORY_H
#define GLVIEWER_FRACTAL_TREEFACTORY_H

#include <memory>

#include <fractal_tree.h>
#include <fractal_stochastictree.h>

namespace artifax {
namespace fractal {

//class FractalTree;
//class StochasticFractalTree;

class FractalTreeFactory {
public:
  static std::unique_ptr<FractalTree> makeBinaryTree(unsigned int iteration = 0);
  static std::unique_ptr<FractalTree> makeSierpinskiTriangle(unsigned int nIterations = 0);
  static std::unique_ptr<FractalTree> makeFractalPlant(unsigned int nIterations = 0);

  static std::unique_ptr<StochasticFractalTree> makeStochasticFractalPlant(unsigned int nIterations = 0, unsigned int seed = 0);
};

} // end package namespace
} // end enterprise namespace

#endif // GLVIEWER_FRACTAL_TREEFACTORY_H
