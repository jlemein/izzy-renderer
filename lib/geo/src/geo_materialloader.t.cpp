//
// Created by jlemein on 01-03-21.
//
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
//using namespace lsw::geo;
using json = nlohmann::json;

class MaterialLoaderTest : public testing::Test {
public:
};

TEST_F(MaterialLoaderTest, ShouldLoadArray) {
  auto j = json::parse("{ \"name\": \"Jeffrey\"}");

  EXPECT_STREQ(j["name"].get<std::string>().c_str(), "Jeffrey");
}

TEST_F(MaterialLoaderTest, ParseColor) {
  auto j = json::parse("{ \"color\": [ 1, 2, 3 ] }");

  auto color = j["color"];
  EXPECT_EQ(color.size(), 3);
  EXPECT_FALSE(color.empty());
//  EXPECT_EQ([0].get<float>(), 1.0F);
}
