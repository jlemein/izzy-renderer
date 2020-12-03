//
// Created by jlemein on 14-11-20.
//
#include <fractal_stochastictree.h>
using namespace artifax::fractal;

StochasticFractalTree::StochasticFractalTree(const std::string &initiator,
                                             State &state, unsigned int seed)
    : m_value(initiator), m_state(state), m_seed(seed), m_generator(seed), m_distribution(0.0F, 1.0F) {}

void StochasticFractalTree::addRule(char matches,
                                    const std::string &replacement,
                                    float probability) {
  m_rules[matches].emplace_back(
      StochasticRule{matches, replacement, probability});
}

void StochasticFractalTree::addDrawCommand(char matches,
                                           const DrawCommand &drawFunction) {
  m_drawCommands[matches] = drawFunction;
}

void StochasticFractalTree::iterate(int n) {
  for (int i = 0; i < n; ++i) {
    iterateOnce();
  }
}

float StochasticFractalTree::generateRandomNumber() {
  return m_distribution(m_generator);
}

void StochasticFractalTree::iterateOnce() {
  std::string evaluated = "";

  for (char v : m_value) {  // loops over characters in string

    if (m_rules.count(v) <= 0) {
      evaluated += v;
      continue;
    }

    auto &ruleSet = m_rules.at(v);
    float r = generateRandomNumber();
    float sum = 0.0F;
    for (auto rule : ruleSet) {
      sum += rule.probability;

      if (sum >= r) {
        evaluated += rule.replacement;
        break;
      }
    }
  }

  m_value = evaluated;
}

void StochasticFractalTree::draw() {
  for (int i = 0; i < m_value.length(); ++i) {
    if (m_drawCommands.count(m_value[i]) > 0) {
      m_drawCommands.at(m_value[i])(m_state);
    }
  }
}

const std::string &StochasticFractalTree::getValue() const { return m_value; }

const State& StochasticFractalTree::getState() const {
  return m_state;
}

std::ostream &operator<<(std::ostream &os, const StochasticFractalTree &tree) {
  os << tree.getValue();
  return os;
}