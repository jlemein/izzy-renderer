//
// Created by jlemein on 10-11-20.
//
#include <fractal_treefactory.h>

#include <fractal_drawcommands.h>
#include <fractal_tree.h>
#include <fractal_stochastictree.h>
#include <fractal_state.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>
using namespace artifax::fractal;

std::unique_ptr<FractalTree>
FractalTreeFactory::makeBinaryTree(unsigned int iteration) {
  State state;
  auto tree = std::make_unique<FractalTree>(std::string{"0"}, state);
  tree->addRule('0', "1[0]0");
  tree->addRule('1', "11");

  if (iteration > 0) {
    tree->iterate(iteration);
  }

  float globalScale = 1.00F;
  FractalTree::DrawCommands drawCommands = {{'0', DrawForward(0.1F * globalScale)},
                               {'1', DrawForward(0.2F * globalScale)},
                               {'[', PushRotate(0.25 * 3.1415F)},
                               {']', PopRotate(-0.25 * 3.1415F)}};
  tree->addCommands(drawCommands);
  return tree;
}

std::unique_ptr<FractalTree>
FractalTreeFactory::makeSierpinskiTriangle(unsigned int nIterations) {
  State state;
  state.radians = (120.0F / 180.0F) * 3.1415F;
  auto sierpinski = std::make_unique<FractalTree>("F-G-G", state);

  sierpinski->addRule('F', "F-G+F+G-F");
  sierpinski->addRule('G', "GG");

  sierpinski->addDrawCommand('F', DrawForward());
  sierpinski->addDrawCommand('G', DrawForward());
  sierpinski->addDrawCommand('+', Rotate(M_PI * (120.0F / 180.0F)));
  sierpinski->addDrawCommand('-', Rotate(M_PI * (-120.0F / 180.0F)));

  sierpinski->iterate(nIterations);

  return sierpinski;
}

std::unique_ptr<FractalTree> FractalTreeFactory::makeFractalPlant(unsigned int nIterations) {
  State state;
  state.radians = 25.0F;
  auto plant = std::make_unique<FractalTree>("X", state);

  plant->addRule('X', "F+[[X]-X]-F[-FX]+X");
  plant->addRule('F', "FF");

  plant->addDrawCommand('F', DrawForward());
  plant->addDrawCommand('-', Rotate(glm::radians(25.0F)));
  plant->addDrawCommand('+', Rotate(glm::radians(-25.0F)));
  plant->addDrawCommand('[', PushState());
  plant->addDrawCommand(']', PopState());

  plant->iterate(nIterations);

  return plant;
}

std::unique_ptr<StochasticFractalTree> FractalTreeFactory::makeStochasticFractalPlant(unsigned int nIterations, unsigned int seed) {
  State state;
  state.radians = 25.0F;
  auto plant = std::make_unique<StochasticFractalTree>("X", state);

  plant->addRule('X', "F+[[X]-X]-F[-FX]+X");
  plant->addRule('F', "FF", 0.5);
  plant->addRule('F', "FFF", 0.5);

  plant->addDrawCommand('F', DrawForward());
  plant->addDrawCommand('-', Rotate(glm::radians(25.0F)));
  plant->addDrawCommand('+', Rotate(glm::radians(-25.0F)));
  plant->addDrawCommand('[', PushState());
  plant->addDrawCommand(']', PopState());

  plant->iterate(nIterations);

  return plant;
}