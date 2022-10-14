#include <gtest/gtest.h>
#include <izzgl_material.h>
using namespace izz::gl;

class MaterialTest : public testing::Test {
public:
};

TEST_F(MaterialTest, IsMoveConstructible) {
  static_assert(std::is_copy_constructible_v<const geo::Material> && std::is_move_constructible_v<const geo::Material>, "WOOW");

  izz::gl::Material m;
  m.ambient = glm::vec3(1.0F, 0.4F, 0.1F);
  m.vertexShader = "vertex-shader";

  auto moved = std::move(m);

  EXPECT_EQ(moved.ambient, glm::vec3(1.0F, 0.4F, 0.1F));
  EXPECT_STREQ(moved.vertexShader.c_str(), "vertex-shader");
}
