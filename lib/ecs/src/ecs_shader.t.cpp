//
// Created by jlemein on 10-01-21.
//
#include <ecs_shader.h>

#include <entt/entt.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
using namespace affx;
using namespace testing;
using namespace affx::ecs;

class ShaderTest : public ::testing::Test {};

//
TEST_F(ShaderTest, ShouldAssignShaderFiles) {
  entt::registry reg;
  auto e = reg.create();

  Shader::UniformProperties props;
  Shader shader{"fileA.shader", "fragmentShader.shader", props};
  reg.emplace<Shader>(e, shader);

  EXPECT_STREQ(reg.get<Shader>(e).vertexShaderFile.c_str(), "fileA.shader");
  EXPECT_STREQ(reg.get<Shader>(e).fragmentShaderFile.c_str(),
               "fragmentShader.shader");
}

TEST_F(ShaderTest, ShouldSetAndGetProperties) {
  entt::registry reg;
  auto e = reg.create();

  ecs::UberMaterialData uber{.diffuse = glm::vec3(0.5F, 0.2F, 1.0F),
                             .specular = glm::vec3(1.0F, -4.0F, 34.0F),
                             .ambient = glm::vec3(0.0F, 0.1F, 0.20F)};

  reg.emplace<ecs::Shader>(e, "assets/shaders/uber.vert.spv",
                           "assets/shaders/uber.frag.spv");
  reg.get<ecs::Shader>(e).setProperty(ecs::UberMaterialData::PARAM_NAME, uber);

  auto data = reg.get<ecs::Shader>(e).getProperty<UberMaterialData>();

  EXPECT_EQ(data->diffuse.r, uber.diffuse.r);
  EXPECT_EQ(data->diffuse.g, uber.diffuse.g);
  EXPECT_EQ(data->diffuse.b, uber.diffuse.b);
  EXPECT_EQ(data->specular.r, uber.specular.r);
  EXPECT_EQ(data->specular.g, uber.specular.g);
  EXPECT_EQ(data->specular.b, uber.specular.b);
  EXPECT_EQ(data->ambient.r, uber.ambient.r);
  EXPECT_EQ(data->ambient.g, uber.ambient.g);
  EXPECT_EQ(data->ambient.b, uber.ambient.b);
}