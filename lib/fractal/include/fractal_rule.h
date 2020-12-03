//
// Created by jlemein on 14-11-20.
//

#ifndef GLVIEWER_FRACTAL_RULE_H
#define GLVIEWER_FRACTAL_RULE_H

#include <string>

class IRule {
public:
  virtual bool matches(char ch) const = 0;
  virtual std::string operator()(char ch) const = 0;
};

class Rule : public IRule {
private:
  char mMatches;
  std::string mReplaced;

public:
  Rule(char matches, std::string replaced)
      : mMatches(matches), mReplaced(replaced) {}

  bool matches(char ch) const override {
    return mMatches == ch;
  }

  std::string operator()(char ch) const override {
    return mReplaced;
  }
};

#endif // GLVIEWER_FRACTAL_RULE_H
