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
  uniforms.addIntArray("intArray", {0, 1, 2}, -1);

  const float* pData = reinterpret_cast<float*>(uniforms.floatArrayValues.at("hello5")->m_data);
  EXPECT_NEAR(pData[0], 1.0F, 0.01F);
  EXPECT_NEAR(pData[1], 2.0F, 0.01F);
}

TEST_F(MaterialTest, Given_HasIntArray_When_RequestData_Then_RetrievesData) {
  UniformProperties uniforms(3*sizeof(int32_t), 1);
  uniforms.addIntArray("intArray", {1920, 1080, 2}, -1);

  const int32_t* pData = reinterpret_cast<int32_t*>(uniforms.intArrayValues.at("intArray")->m_data);
  EXPECT_EQ(pData[0], 1920);
  EXPECT_EQ(pData[1], 1080);
  EXPECT_EQ(pData[2], 2);
}

TEST_F(MaterialTest, Given_HasFloatArray_When_RequestData_Then_RetrievesData) {
  UniformProperties uniforms(24, 5);

  uniforms.addFloat("hello1", 10.0F, -1);
//  uniforms.addFloat("hello2", 11.0F, -1);
//  uniforms.addFloat("hello3", 12.0F, -1);
//  uniforms.addFloat("hello4", 13.0F, -1);
//  uniforms.addFloatArray("hello5", {1.0F, 2.0F}, -1);

  const float* pData = reinterpret_cast<float*>(uniforms.floatValues.at("hello1")->m_data);
  EXPECT_NEAR(*pData, 10.0F, 0.01F);
}