//
// Created by jlemein on 25-02-21.
//
#pragma once

#include <izz_fontsystem.h>
#include <izz_resourcemanager.h>
#include <izzgl_deferredrenderablefactory.h>
#include <izzgl_entityfactory.h>
#include <izzgl_exrloader.h>
#include <izzgl_materialsystem.h>
#include <izzgl_meshsystem.h>
#include <izzgl_sceneloader.h>
#include <izzgl_stbtextureloader.h>
#include <izzgl_texturesystem.h>
#include <izzgui_componenteditor.h>
#include <izzgui_docklayout.h>
#include <izzgui_guisystem.h>
#include <izzgui_mainmenu.h>
#include <izzgui_materialnavigator.h>
#include <izzgui_stats.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include "izzgui_cubemaptool.h"

namespace izz {
namespace geo {
class Texture;
class Scene;
}  // namespace geo

/**!
 * @brief Higher level resource manager specifically focused on loading
 * entities from the geometry package. The geometry package contains all higher
 * level entities, independent of the rendering system used.
 */
class Izzy {
 private:
  entt::registry registry;

 public:
  std::shared_ptr<izz::gui::GuiSystem> guiSystem{nullptr};
  std::shared_ptr<izz::ResourceManager> resourceManager{nullptr};
  std::shared_ptr<izz::gl::TextureSystem> textureSystem{nullptr};
  std::shared_ptr<izz::gl::MaterialSystem> materialSystem{nullptr};
  std::shared_ptr<izz::gl::MeshSystem> meshSystem{nullptr};
  std::shared_ptr<izz::gl::RenderSystem> renderSystem{nullptr};
  std::shared_ptr<izz::gl::SceneLoader> sceneLoader{nullptr};
  std::shared_ptr<izz::EntityFactory> entityFactory{nullptr};
  std::shared_ptr<izz::FontSystem> fontSystem{nullptr};  // @todo make part of izz::gui
  std::shared_ptr<izz::gl::LightSystem> lightSystem{nullptr};

  static std::shared_ptr<Izzy> CreateSystems() {
    auto izz = std::make_shared<Izzy>();
    izz->guiSystem = std::make_shared<izz::gui::GuiSystem>();
    izz->resourceManager = std::make_shared<izz::ResourceManager>();
    izz->textureSystem = std::make_shared<izz::gl::TextureSystem>();
    izz->textureSystem->setTextureLoader(".exr", std::make_unique<izz::gl::ExrLoader>(true));
    izz->textureSystem->setTextureLoader(izz::gl::ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<izz::gl::StbTextureLoader>(true));
    izz->resourceManager->setTextureSystem(izz->textureSystem);
    izz->materialSystem = std::make_shared<izz::gl::MaterialSystem>(izz->registry, izz->resourceManager);
    izz->meshSystem = std::make_shared<izz::gl::MeshSystem>();
    izz->resourceManager->setMaterialSystem(izz->materialSystem);
    izz->lightSystem = std::make_shared<izz::gl::LightSystem>(izz->registry, *izz->materialSystem, *izz->meshSystem);
    izz->renderSystem = std::make_shared<izz::gl::RenderSystem>(izz->registry, izz->resourceManager, izz->materialSystem, izz->textureSystem,
                                                                izz->meshSystem, izz->lightSystem);
    izz->lightSystem->setRenderSystem(*izz->renderSystem);
    izz->entityFactory = std::make_shared<izz::EntityFactory>(izz->registry, *izz->renderSystem, *izz->materialSystem, *izz->meshSystem);
    izz->sceneLoader = std::make_shared<izz::gl::SceneLoader>(izz->textureSystem, izz->materialSystem);
    izz->resourceManager->setSceneLoader(izz->sceneLoader);

    izz->fontSystem = std::make_shared<izz::FontSystem>();
    //    izz->fontSystem->addFont("fonts/SegoeUi.ttf", 16);
    izz->fontSystem->addFont("fonts/DroidSans.ttf", 18);
    izz->guiSystem = make_shared<gui::GuiSystem>(izz->fontSystem);

    return izz;
  }

  entt::registry& getRegistry() {
    return registry;
  }

  void setupUserInterface() {
    guiSystem->addDialog(std::make_shared<izz::gui::MainMenu>(*this));
    guiSystem->addDialog(std::make_shared<izz::gui::StatsDialog>());
    guiSystem->addDialog(std::make_shared<izz::gui::CubeMapTool>(materialSystem, textureSystem, meshSystem, entityFactory));

    guiSystem->addDialog(std::make_shared<izz::gui::SceneNavigator>(entityFactory));
    guiSystem->addDialog(std::make_shared<izz::gui::MaterialInspector>(materialSystem));
    guiSystem->addDialog(std::make_shared<izz::gui::MaterialEditor>(materialSystem, entityFactory));
    guiSystem->addDialog(std::make_shared<izz::gui::MaterialNavigator>(entityFactory, materialSystem));
    guiSystem->addDialog(std::make_shared<izz::gui::ComponentEditor>(*this));

    guiSystem->setLayout(std::make_unique<izz::gui::DockLayout>(0.2, 0.2, 0.2, 0.5));
  }

 private:
//  std::shared_ptr<izz::gl::MaterialSystem> m_materialSystem{nullptr};
//  std::shared_ptr<izz::gl::SceneLoader> m_sceneLoader{nullptr};
//  std::shared_ptr<izz::gl::TextureSystem> m_textureSystem;
};

}  // namespace izz
