//
// Created by jlemein on 06-11-20.
//

#ifndef GLVIEWER_VIEWER_TEXTURESYSTEM_H
#define GLVIEWER_VIEWER_TEXTURESYSTEM_H

#include <ecs_interactable.h>
#include <ecs_rendersubsystem.h>
#include <entt/entt.hpp>

class TextureSystem : public affx::ecs::IViewerInteractable, public affx::ecs::IRenderSubsystem {
public:

  virtual ~TextureSystem() override = default;

  void init(entt::registry&) override;
  void update(entt::registry&, float time, float dt) override;
  void render(const entt::registry& registry) override {}

  void onRender(const entt::registry &registry, entt::entity entity) override;
};

#endif // GLVIEWER_VIEWER_TEXTURESYSTEM_H
