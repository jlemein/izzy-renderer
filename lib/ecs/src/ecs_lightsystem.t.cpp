//
// Created by jlemein on 06-02-21.
//
#include <ecs_lightsystem.h>
#include <gtest/gtest.h>
using namespace lsw::ecs;

class LightSystemTest : public ::testing::Test {};

TEST_F(LightSystemTest, SizeOfForwardLighting) {
  UDirectionalLight s;
  float* pData = reinterpret_cast<float*>(&s);
  s.intensity = 5.0F;
  EXPECT_EQ(sizeof(UDirectionalLight), 8*sizeof(float));

  EXPECT_EQ(pData[3], 5.0F);
}