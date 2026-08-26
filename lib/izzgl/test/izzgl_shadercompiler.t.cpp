//
// Created by jeffrey on 12-10-22.
//
#include <gtest/gtest.h>
#include <boost/functional/hash.hpp>
#include <regex>
#include <string>
#include <iostream>

class ShaderCompilerTest : public testing::Test {
 public:
};


TEST_F(ShaderCompilerTest, Given_IncludeString_When_Parsed_ShouldReturnFile2) {
  std::vector<std::string> lines = {"#include <iostream>", " #include\"abcd\"}", "#include  'hello.cpp'"};

  std::regex rgx("(<.+>)|('.+')|(\".+\")");
  std::smatch match;

  for (auto s : lines) {
    if (std::regex_search(s, match, rgx)) {
      std::string m =  match[0];
      std::cout << "Match: " << m.substr(1, m.size()-2) << std::endl;
    }
  }
}

TEST_F(ShaderCompilerTest, Given_IncludeString_When_Parsed_ShouldReturnFile) {
  std::string s = "#include <iostream>";
  std::regex rgx("(?<=<).*?(?=>)");
  std::smatch match;

  if (std::regex_search(s, match, rgx)) {
    std::cout << "Match: " << match[1] << std::endl;
  } else {
    std::cout << "No match" << std::endl;
  }
}
