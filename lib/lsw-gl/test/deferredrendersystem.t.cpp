//
// Created by jeffrey on 16-12-21.
//
#include <gtest/gtest.h>
#include <gl_deferredrenderer.h>
#include <memory>
#include <entt/entt.hpp>
#include <gl_rendersystem.h>
using namespace izz::gl;

class DeferredRendererTest : public ::testing::Test {
 public:
};

TEST_F(DeferredRendererTest, TestDeferredInitialization) {
  entt::registry registry;
  RenderSystem renderSystem(registry, nullptr, nullptr);

  entt::entity e = registry.create();
  DeferredRenderable r1;
  std::cout << "Render state id: " << r1.renderStateId << std::endl;
  registry.emplace<DeferredRenderable>(e, r1);

  auto& r = registry.get<DeferredRenderable>(e);
  std::cout << "Render state id: " << r.renderStateId << std::endl;
  EXPECT_GE(r.renderStateId, 0);
}