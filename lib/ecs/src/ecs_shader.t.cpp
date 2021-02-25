//
// Created by jlemein on 10-01-21.
//
#include <geo_material.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
using namespace affx;
using namespace testing;
using namespace affx::geo;

class ShaderTest : public ::testing::Test {};

//
TEST_F(ShaderTest, ShouldAssignShaderFiles) {
  entt::registry reg;
  auto e = reg.create();

  Material::UniformProperties props;
  Material material{.vertexShader = "fileA.shader",
                    .fragmentShader = "fragmentShader.shader",
                    .properties = props};
  reg.emplace<Material>(e, material);

  EXPECT_STREQ(reg.get<Material>(e).vertexShader.c_str(), "fileA.shader");
  EXPECT_STREQ(reg.get<Material>(e).fragmentShader.c_str(),
               "fragmentShader.shader");
}

TEST_F(ShaderTest, ShouldSetAndGetProperties) {
  entt::registry reg;
  auto e = reg.create();

  UberMaterialData uber{.diffuse = glm::vec4(0.5F, 0.2F, 1.0F, .0F),
                        .specular = glm::vec4(1.0F, -4.0F, 34.0F, .0F),
                        .ambient = glm::vec4(0.0F, 0.1F, 0.20F, 0.0F)};

  reg.emplace<Material>(
      e, Material{.vertexShader = "assets/shaders/uber.vert.spv",
                  .fragmentShader = "assets/shaders/uber.frag.spv"});
  reg.get<Material>(e).setProperty(UberMaterialData::PARAM_NAME, uber);

  auto data = reg.get<Material>(e).getProperty<UberMaterialData>();

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