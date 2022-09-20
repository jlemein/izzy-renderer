#include <fractal_tree.h>
#include <iostream>
using namespace izz::fractal;


FractalTree::FractalTree(std::string initiator, const State& state)
    : m_value(initiator), m_state(state) {
//  m_state.radians = (120.0F / 180.0F) * 3.1415F;
}

void FractalTree::addRule(char matches, std::string result) {
  m_rules.push_back(Rule(matches, result));
}

//void FractalTree::addRule(const IRule* rule) {
//  mRules.push_back(rule);
//}

void FractalTree::addDrawCommand(char matches, std::function<void(State&)> drawFunction) {
  m_drawCommands[matches] = drawFunction;
}

void FractalTree::iterate(int n) {
  for (int i=0; i<n; ++i) {
    iterateOnce();
  }
}

FractalTree& FractalTree::iterateOnce() {
  std::string evaluated = "";

  for (char v : m_value) {
    bool hasMatched {false};

    for (auto& rule : m_rules) {
      if (rule.matches(v)) {
        evaluated += rule(v);
        hasMatched = true;
        break;
      }
    }

    if (!hasMatched) {
      evaluated += v;
    }
  }

  m_value = evaluated;
  return *this;
}

void FractalTree::draw() {
  for (int i = 0; i < m_value.length(); ++i) {
    if (m_drawCommands.count(m_value[i]) > 0) {
      m_drawCommands.at(m_value[i])(m_state);
    }
  }
}

const std::string& FractalTree::getValue() const {
  return m_value;
}

std::ostream& operator<<(std::ostream& os, const FractalTree& tree) {
  os << tree.getValue();
  return os;
}