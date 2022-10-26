//
// Created by jeffrey on 12-10-22.
//
#include <gtest/gtest.h>
#include <boost/functional/hash.hpp>
#include "geo_capabilities.h"
#include <izzgl_irendercapabilityselector.h>
using namespace izz;

class RenderCapabilitiesTest : public testing::Test {
 public:
};

TEST_F(RenderCapabilitiesTest, Given_SameRenderCapabilities_When_ComputeHash_Then_ShouldHaveSameHash) {
  geo::Capabilities cap1;
  cap1.flags.insert("Hello");
  cap1.flags.insert("Bye");

  geo::Capabilities cap2;
  cap2.flags.insert("Bye");
  cap2.flags.insert("Hello");

  auto hash1 = std::hash<gl::RenderCapabilities>{}(cap1);
  auto hash2 = std::hash<gl::RenderCapabilities>{}(cap2);
  EXPECT_EQ(hash1, hash2) << "Hash values should match.";
  EXPECT_TRUE(cap1 == cap2);
}

TEST_F(RenderCapabilitiesTest, Give_DifferentCapabilities_When_CompareHash_Then_ShouldBeDifferent) {
  geo::Capabilities cap1, cap2;
  cap1.flags.insert("A");
  cap1.flags.insert("B");
  cap1.settings["time"] = "12:00";
  cap1.settings["age"] = "20";

  cap2 = cap1;
  cap2.flags.erase("A");

  auto hash1 = std::hash<gl::RenderCapabilities>{}(cap1);
  auto hash2 = std::hash<gl::RenderCapabilities>{}(cap2);
  EXPECT_NE(hash1, hash2) << "Hash values should be different.";
  EXPECT_FALSE(cap1 == cap2);
}

TEST_F(RenderCapabilitiesTest, Give_TwoCapabilities_When_Merging_Then_ShouldHaveMergedCapabilities) {
  geo::Capabilities cap1, cap2;
  cap1.flags.insert("A");
  cap1.flags.insert("B");
  cap1.settings["time"] = "12:00";
  cap1.settings["age"] = "20";

  cap2.flags.insert("B");
  cap2.flags.insert("C");
  cap2.settings["time"] = "12:01";
  cap2.settings["happy"] = "true";

  auto cap3 = cap1.add(cap2);
  EXPECT_TRUE(cap3.flags.contains("A"));
  EXPECT_TRUE(cap3.flags.contains("B"));
  EXPECT_TRUE(cap3.flags.contains("C"));
  EXPECT_STREQ(cap3.settings["time"].c_str(), "12:01") << "When merging A with B, B's values overwrite A in case of collision.";
  EXPECT_STREQ(cap3.settings["age"].c_str(), "20");
  EXPECT_STREQ(cap3.settings["happy"].c_str(), "true");
}