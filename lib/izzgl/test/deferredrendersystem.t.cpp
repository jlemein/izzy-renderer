//
// Created by jeffrey on 16-12-21.
//
#include <gl_deferredrenderer.h>
#include <gtest/gtest.h>
#include <izzgl_rendersystem.h>
#include <entt/entt.hpp>
#include <memory>
#include "izzgl_deferredrenderablefactory.h"
using namespace izz::gl;

class DeferredRendererTest : public ::testing::Test {
 public:
};

TEST_F(DeferredRendererTest, TestDeferredInitialization) {
  entt::registry registry;
  RenderSystem renderSystem(registry, nullptr);

  entt::entity e = registry.create();
  DeferredRenderable r1;
  std::cout << "Render state id: " << r1.renderStateId << std::endl;
  registry.emplace<DeferredRenderable>(e, r1);

  auto& r = registry.get<DeferredRenderable>(e);
  std::cout << "Render state id: " << r.renderStateId << std::endl;
  EXPECT_GE(r.renderStateId, 0);
}

struct MyComponent {
  int value;
};

void onConstruct(entt::registry& registry, entt::entity e) {
  MyComponent& r = registry.get<MyComponent>(e);

  // if render state exists
  std::cout << "Hello On construct - renderstateid = " << r.value << std::endl;
  r.value = 100;
}

TEST_F(DeferredRendererTest, ReproduceBug) {
  spdlog::info("TEST2");
  entt::registry registry;
  registry.on_construct<MyComponent>().connect<onConstruct>();

  int rsid = 12;
//  int materialId = 10;
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  registry.emplace<MyComponent>(registry.create(), MyComponent{rsid});//, materialId});
  auto view = registry.view<const MyComponent>();
  spdlog::info("=== DEFERRED RENDER ===");
  for (entt::entity e : view) {
      //      auto r = view.get<const DeferredRenderable>(e);
      auto rid = view.get<const MyComponent>(e).value;
      spdlog::info("value for for (e: {}) = {}", static_cast<int>(e), rid);
  }
}