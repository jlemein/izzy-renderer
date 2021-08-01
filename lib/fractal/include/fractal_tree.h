//
// Created by jlemein on 10-11-20.
//

#ifndef GLVIEWER_FRACTAL_TREE_H
#define GLVIEWER_FRACTAL_TREE_H

#include <functional>
#include <iosfwd>
#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>

#include <fractal_rule.h>
#include <fractal_state.h>

namespace lsw {
namespace fractal {

/**!
 * @brief A basic Fractal tree representation to model L-systems.
 */
class FractalTree {
private:
  std::string m_value;
  std::vector<Rule> m_rules;
  std::unordered_map<char, std::function<void(State &)>> m_drawCommands;
  State m_state;        // todo this is dependent on type of fractal tree and draw commands
                        // possible refactoring out to template type -- FractalTree<State>

public:
  using DrawCommand = std::function<void(State &)>;
  using DrawCommands = std::unordered_map<char, DrawCommand>;

  FractalTree(std::string initiator, const State& state);

  /**!
   * Adds rules
   * @param from
   * @param to
   */
  void addRule(char from, std::string to);

  void iterate(int n = 1);
  FractalTree &iterateOnce();

  const std::string &getValue() const;

  void addDrawCommand(char matches, DrawCommand command);

  void addCommands(DrawCommands &commands) { m_drawCommands = commands; }

  void draw();

  const State& getState() const {
    return m_state;
  }

  friend std::ostream &operator<<(std::ostream &os, const FractalTree &tree);
};

} // end package
} // end enterprise namespace

#endif // GLVIEWER_FRACTAL_TREE_H
