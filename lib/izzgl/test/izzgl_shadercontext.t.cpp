//
// Created by jeffrey on 12-10-22.
//
#include <gtest/gtest.h>
#include <izzgl_shadersource.h>
#include <string>
using namespace izz::gl;

class ShaderContextTest : public testing::Test {
 public:
};

TEST_F(ShaderContextTest, Given_ContextWithShaderRoot_When_PathIsAbsolute_Then_PathIsResolvedFromShaderRoot) {
  // GIVEN
  ShaderContext context;
  context.root = "/mydir/abc/";

  // WHEN
  std::filesystem::path p = "/common/file.glsl";

  // THEN
  EXPECT_STREQ(ShaderContext::ResolvePath(p, &context).string().c_str(), "/mydir/abc/common/file.glsl");
}

TEST_F(ShaderContextTest, Given_ContextWithShaderRoot_When_PathIsRelative_Then_PathIsUnchanged) {
  // GIVEN
  ShaderContext context;
  context.root = "/mydir/abc/";

  // WHEN
  std::filesystem::path p = "common/file.glsl";

  // THEN
  EXPECT_STREQ(ShaderContext::ResolvePath(p, &context).string().c_str(), "common/file.glsl");
}
