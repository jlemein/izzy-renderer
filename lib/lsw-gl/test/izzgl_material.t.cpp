//
// Created by jeffrey on 2-5-22.
//
#include <GL/glew.h>
#include <gtest/gtest.h>
#include "izzgl_material.h"
using namespace izz::gl;

class MaterialTest : public ::testing::Test {
 public:
};

TEST_F(MaterialTest, UnscopedUniforms_Test) {
  UniformProperties uniforms(24, 5);

  uniforms.addFloat("hello1", 10.0F, -1);
  uniforms.addFloat("hello2", 11.0F, -1);
  uniforms.addFloat("hello3", 12.0F, -1);
  uniforms.addFloat("hello4", 13.0F, -1);
  uniforms.addFloatArray("hello5", {1.0F, 2.0F}, -1);

  const float* pData = reinterpret_cast<float*>(uniforms.floatArrayValues.at("hello5")->m_data);
  EXPECT_NEAR(pData[0], 1.0F, 0.01F);
  EXPECT_NEAR(pData[1], 2.0F, 0.01F);
}

TEST_F(MaterialTest, UnscopedUniforms_Test2) {
  UniformProperties uniforms(24, 5);

  uniforms.addFloat("hello1", 10.0F, -1);
//  uniforms.addFloat("hello2", 11.0F, -1);
//  uniforms.addFloat("hello3", 12.0F, -1);
//  uniforms.addFloat("hello4", 13.0F, -1);
//  uniforms.addFloatArray("hello5", {1.0F, 2.0F}, -1);

  const float* pData = reinterpret_cast<float*>(uniforms.floatValues.at("hello1")->m_data);
  EXPECT_NEAR(*pData, 10.0F, 0.01F);
}