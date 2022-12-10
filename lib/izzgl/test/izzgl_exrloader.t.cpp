//
// Created by jeffrey on 12-10-22.
//
#include <gtest/gtest.h>
#include <izzgl_exrloader.h>
#include <izzgl_stbtextureloader.h>
#include <izz_texture.h>
using namespace izz::gl;

class RenderCapabilitiesTest : public testing::Test {
 public:
};

TEST_F(RenderCapabilitiesTest, Given_ExrEnvironmentMap_When_ReadingAndWriting_Then_ResultShouldBeSame) {
  izz::gl::ExrLoader exr;
  izz::gl::StbTextureLoader stb;

  auto path = "../resources/textures/park_parking_4k.exr";
  auto outputPath = "park_parking.png";
  Texture t = exr.loadImage(path);
  stb.writeTexture(&t, outputPath);
}
