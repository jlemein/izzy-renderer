//
// Created by jlemein on 14-11-20.
//

#ifndef GLVIEWER_FRACTAL_STOCHASTICTREE_H
#define GLVIEWER_FRACTAL_STOCHASTICTREE_H

#include <fractal_state.h>

#include <string>
#include <functional>
#include <iosfwd>
#include <random>

namespace izz {
namespace fractal {

struct StochasticRule {
  char matching;
  std::string replacement;
  float probability {1.0F};
};

class StochasticFractalTree {
public:
  using DrawCommand = std::function<void(State &)>;
  using DrawCommands = std::unordered_map<char, DrawCommand>;
  using RuleSet = std::unordered_map<char, std::vector<StochasticRule>>;

  StochasticFractalTree(const std::string& initiator, State& state, unsigned int seed = 0);

  void addRule(char matches, const std::string& replace, float probability = 1.0F);

  void iterate(int n = 1);

  const std::string& getValue() const;

  void addDrawCommand(char matches, const DrawCommand& command);

  void draw();

  const State& getState() const;

  friend std::ostream &operator<<(std::ostream &os, const StochasticFractalTree &tree);

private:
  std::string m_value {""};
  unsigned int m_seed {0};
  State m_state;

  RuleSet m_rules;
  DrawCommands m_drawCommands;
  std::mt19937 m_generator;
  std::uniform_real_distribution<float> m_distribution;

  float generateRandomNumber();

  void iterateOnce();
};

} // end package naming
} // end enterprise name

#endif // GLVIEWER_FRACTAL_STOCHASTICTREE_H
