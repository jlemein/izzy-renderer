//
// Created by jlemein on 06-02-21.
//
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <ecs_transformsystem.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <gtest/gtest.h>
using namespace izz::ecs;

class TransformSystemTest : public ::testing::Test {};

/*
 * Tests if the transform system will update the world transform for
 * two entities: both root entities, with one having a relationship component
 * and the other not. No child/parent relationships involved.
 */
TEST_F(TransformSystemTest, SingleEntity_ShouldHave_WorldIsLocalTransform) {
  entt::registry registry;
  TransformSystem ts(registry);

  // first entity is root entity with a translation of -10, 2, -9.4
  auto e1 = registry.create();
  registry.emplace<Transform>(
      e1, Transform{.localTransform = glm::translate(
                        glm::mat4{1.0F}, glm::vec3(-10.0F, 2.0F, -9.4F))});
  registry.emplace<Relationship>(e1);

  // second entity is root, has no relationship component, with translation of 1, -54, 10
  auto e2 = registry.create();
  registry.emplace<Transform>(
      e2, Transform{.localTransform = glm::translate(
                        glm::mat4{1.0F}, glm::vec3(1.0F, -54.0F, 10.0F))});

  ts.init();             // should sort the transform system
  ts.update(1.0F, 1.0F); // should propagate local transform to world transforms

  auto &t1 = registry.get<Transform>(e1);
  EXPECT_EQ(t1.worldTransform[3].x, -10.0F);
  EXPECT_EQ(t1.worldTransform[3].y, 2.0F);
  EXPECT_EQ(t1.worldTransform[3].z, -9.4F);

  auto &t2 = registry.get<Transform>(e2);
  EXPECT_EQ(t2.worldTransform[3].x, 1.0F);
  EXPECT_EQ(t2.worldTransform[3].y, -54.0F);
  EXPECT_EQ(t2.worldTransform[3].z, 10.0F);
}

/*
 * Tests if the transform system will update the world transform for
 * two entities: both root entities, with one having a relationship component
 * and the other not. No child/parent relationships involved.
 */
TEST_F(TransformSystemTest, ShouldTransformChildOfParent) {
  entt::registry registry;
  TransformSystem ts(registry);

  auto parent = registry.create();
  auto child = registry.create();
  // parent entity has translation 4, 5, 6.
  registry.emplace<Transform>(parent, Transform{.localTransform = glm::translate(glm::mat4(1.0F), glm::vec3(4.0, 5.0, 6.0))});
  registry.emplace<Relationship>(parent, Relationship{.children = {child}, .depth = 0});

  // child entity has identity matrix.
  auto& childTransform = registry.emplace<Transform>(child, Transform{.localTransform = glm::mat4(1.0F)});
  registry.emplace<Relationship>(child, Relationship{.parent = parent, .depth = 1});

  ts.init();
  ts.update(1.0F, 1.0F);
  // now child should have the propagated transform of the parent (4, 5, 6)

  EXPECT_EQ(childTransform.worldTransform[3].x, 4.0F);
  EXPECT_EQ(childTransform.worldTransform[3].y, 5.0F);
  EXPECT_EQ(childTransform.worldTransform[3].z, 6.0F);

}

TEST_F(TransformSystemTest, ShouldPropagateTransformMultipleLevels) {
  entt::registry registry;
  TransformSystem ts(registry);

  auto parent = registry.create();
  auto child = registry.create();
  auto childchild = registry.create();

  registry.emplace<Name>(parent, "MyParent");
  registry.emplace<Name>(child, "MyChild");
  registry.emplace<Name>(childchild, "MyChild2");

  // parent entity has translation 4, 5, 6.
  registry.emplace<Transform>(parent, Transform{.localTransform = glm::translate(glm::mat4(1.0F), glm::vec3(4.0, 5.0, 6.0))});
  registry.emplace<Relationship>(parent, Relationship{.children = {child}, .depth = 0});

  // child entity has identity matrix.
  registry.emplace<Transform>(child, Transform{.localTransform = glm::translate(glm::mat4(1.0F), glm::vec3(7.0F, 10.0F, 40.0F))});
  registry.emplace<Relationship>(child, Relationship{.parent = parent, .children = {childchild}, .depth = 1});

  registry.emplace<Transform>(childchild, Transform{.localTransform = glm::translate(glm::mat4(1.0F), glm::vec3(1.0, 2.0, 3.0))});
  registry.emplace<Relationship>(childchild, Relationship{.parent = child, .depth = 2});

  ts.init();
  ts.update(1.0F, 1.0F);
  // now child should have the propagated transform of the parent (4, 5, 6)

  auto& transform = registry.get<Transform>(childchild);
  EXPECT_EQ(transform.worldTransform[3].x, 12.0F);
  EXPECT_EQ(transform.worldTransform[3].y, 17.0F);
  EXPECT_EQ(transform.worldTransform[3].z, 49.0F);

}