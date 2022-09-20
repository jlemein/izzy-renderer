//
// Created by jlemein on 01-08-21.
//
#include <gtest/gtest.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

class SceneGraphHelperTest : public testing::Test {
public:
};

TEST_F(SceneGraphHelperTest, FindDirectionFromLookAt) {
  glm::vec3 eye(.0F, .0F, 200.0F), center(.0F, .0F, .0F), up(.0F, 1.F, .0F);
  auto mat = glm::inverse(glm::lookAt(eye, center, up));

  std::cout << "Matrix: " << std::endl;
  std::cout << mat[0][0] << " \t" << mat[1][0] << " \t" << mat[2][0] << " \t" << mat[3][0] << std::endl
            << mat[0][1] << " \t" << mat[1][1] << " \t" << mat[2][1] << " \t" << mat[3][1] << std::endl
            << mat[0][2] << " \t" << mat[1][2] << " \t" << mat[2][2] << " \t" << mat[3][2] << std::endl
            << mat[0][3] << " \t" << mat[1][3] << " \t" << mat[2][3] << " \t" << mat[3][3] << std::endl;

  auto pos = mat[3];
  EXPECT_FLOAT_EQ(pos.x, eye.x);
  EXPECT_FLOAT_EQ(pos.y, eye.y);
  EXPECT_FLOAT_EQ(pos.z, eye.z);

  auto dir = mat*glm::vec4(0,0,-1,0);
  EXPECT_FLOAT_EQ(dir.x, .0F);
  EXPECT_FLOAT_EQ(dir.y, .0F);
  EXPECT_FLOAT_EQ(dir.z, -1.0F);

}