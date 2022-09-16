//
// Created by jlemein on 17-02-22.
//

#include <ecs_transform.h>
#include <geo_mesh.h>
#include <izzgl_deferredrenderablefactory.h>
#include <izzgl_materialreader.h>
#include <izzgl_materialsystem.h>
#include <izzgl_rendersystem.h>
#include "anim_localrotation.h"
#include "core_util.h"
#include "ecs_firstpersoncontrol.h"
#include "ecs_light.h"
#include "ecs_transformutil.h"
#include "geo_meshutil.h"
#include "geo_scene.h"
#include "gui_iguiwindow.h"
#include "gui_lighteditor.h"
#include "izz_fontsystem.h"
#include "izz_resourcemanager.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_exrloader.h"
#include "izzgl_sceneloader.h"
#include "izzgl_stbtextureloader.h"
#include "izzgl_texturesystem.h"

#include "gui_window.h"
#include "wsp_workspace.h"

#include <ecs_transformutil.h>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
#include "ecs_camera.h"
#include "geo_primitivefactory.h"
#include "gui_mainmenu.h"
#include <izz_izzy.h>
using namespace std;
using namespace lsw;
using namespace izz;
using namespace lsw::geo;
using namespace glm;
using lsw::core::Util;
using lsw::wsp::Workspace;

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]);

namespace {
std::shared_ptr<izz::Izzy> izzy {nullptr};
std::shared_ptr<Workspace> programArguments{nullptr};
}  // namespace

void setupSystems() {
  izzy = Izzy::CreateSystems();

//  resourceManager = make_shared<ResourceManager>();
//
//  auto textureSystem = make_shared<izz::gl::TextureSystem>();
//  textureSystem->setTextureLoader(".exr", std::make_unique<izz::gl::ExrLoader>(true));
//  textureSystem->setTextureLoader(izz::gl::ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<izz::gl::StbTextureLoader>(true));
//  resourceManager->setTextureSystem(textureSystem);
//
//  materialSystem = make_shared<izz::gl::MaterialSystem>(registry, resourceManager);
//  meshSystem = make_shared<izz::gl::MeshSystem>();
//  //  effectSystem = make_shared<gl::EffectSystem>(*sceneGraphHelper, *materialSystem);
//  resourceManager->setMaterialSystem(materialSystem);
//  renderSystem = std::make_shared<izz::gl::RenderSystem>(registry, resourceManager, materialSystem, meshSystem /*, effectSystem*/);
//  sceneGraphHelper = make_shared<izz::SceneGraphHelper>(registry, std::make_unique<gl::DeferredRenderableFactory>(*renderSystem, *materialSystem),
//                                                        materialSystem, meshSystem);
//
//  sceneLoader = make_shared<izz::gl::SceneLoader>(textureSystem, materialSystem);
//  resourceManager->setSceneLoader(sceneLoader);
//
//  fontSystem = make_shared<FontSystem>();
//  fontSystem->addFont("fonts/SegoeUi.ttf", 20);
//  //  fontSystem->addFont("fonts/DroidSans.ttf", 20);
//  guiSystem = make_shared<gui::GuiSystem>(fontSystem);
}

void setupLights() {
  // Sun
  izzy->sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
//
//  // Point light 1
//  auto ptLight1 = izzy->sceneGraph->makePointLight("PointLight 1", glm::vec3(1.F, 1.0F, -1.0F));
//  auto& lightComp = ptLight1.get<ecs::PointLight>();
//  lightComp.intensity = 1.0;
//  lightComp.color = glm::vec3(1.0, 1.0, 1.0);
//  ptLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));
//
//  // Point light 2
//  auto ptLight2 = izzy->sceneGraph->makePointLight("PointLight 2", glm::vec3(-10.F, 1.0F, -1.0F));
//  ptLight2.get<ecs::PointLight>().intensity = 1.4;
//  ptLight2.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));
}

void setupScene() {
  auto scene = izzy->sceneLoader->loadScene(programArguments->sceneFile);

  // post process meshes in scene file
  for (auto& mesh : scene->m_meshes) {
    MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(*mesh);
    MeshUtil::GenerateSmoothNormals(*mesh);
  }

  // add to scene graph
  izzy->sceneGraph->makeScene(*scene, izz::SceneLoaderFlags::All());

  {
    // adding a custom primitive to the scene
    auto plane = izzy->sceneGraph->makeMoveableEntity("Plane");
    auto& mesh = plane.add<Mesh>(PrimitiveFactory::MakePlane("MyPlane", 15, 15));
    auto& meshBuffer = izzy->meshSystem->createMeshBuffer(mesh);
    auto& material = izzy->materialSystem->createMaterial("DeferredStandard_Color");
    mesh.materialId = material.id;
    auto& dr = plane.add<gl::DeferredRenderable>({material.id, meshBuffer.id});
  }
  {
    auto box = izzy->sceneGraph->makeMoveableEntity("Box");
    auto& mesh = box.add<Mesh>(PrimitiveFactory::MakeBox("MyBox", .5, .5));
    auto& meshBuffer = izzy->meshSystem->createMeshBuffer(mesh);
    lsw::ecs::TransformUtil::Translate(box.get<lsw::ecs::Transform>(), glm::vec3(0.2, 0.0, 0.0));
    auto& material = izzy->materialSystem->createMaterial("DeferredStandard_Color");
    mesh.materialId = material.id;
    auto& dr = box.add<gl::DeferredRenderable>({material.id, meshBuffer.id});
  }

  //  MeshUtil::ScaleUvCoords(plane, 3, 3);
  //  auto effect = effectSystem->createEffect("table_cloth");
  //  auto ee = izzy->sceneGraph->addGeometry(plane, material.id);
  //  auto& tf = registry.get<ecs::Transform>(ee);
  //  ecs::TransformUtil::Translate(tf, glm::vec3(0,0,5));
}

void setupUserInterface() {
  izzy->guiSystem->addDialog(make_shared<gui::LightEditor>(izzy->sceneGraph, izzy->fontSystem));
  izzy->guiSystem->addDialog(make_shared<gui::MainMenu>(izzy->sceneGraph));
}

int main(int argc, char* argv[]) {
  using namespace lsw::ecs;

  try {
    programArguments = parseProgramArguments(argc, argv);
    if (programArguments->debugMode) {
      spdlog::set_level(spdlog::level::debug);
    }

    // creates and initializes all resource systems
    izzy = Izzy::CreateSystems();

    auto window = make_shared<gui::Window>(izzy->getRegistry(), izzy->renderSystem, izzy->guiSystem);  // guiSystem);
    window->setWindowSize(1920, 1080);
    window->setTitle(fmt::format("Izzy Renderer: {}", programArguments->sceneFile.filename().string()));
    window->initialize();

    // setup camera
    auto camera = izzy->sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;
    window->setActiveCamera(camera);

    if (programArguments->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      izz::gl::MaterialReader reader(izzy->materialSystem);
      reader.readMaterials(programArguments->materialsFile);
      //      effectSystem->readEffectsFromFile(programArguments->materialsFile);
    }

    setupScene();
    setupLights();
    // visualize point lights using a custom material.
    izzy->renderSystem->getLightSystem().setDefaultPointLightMaterial(izzy->materialSystem->createMaterial("pointlight").id);
    izzy->renderSystem->init(window->getDisplayDetails().windowWidth, window->getDisplayDetails().windowHeight);
    //

    //

        setupUserInterface();

    //    auto grayscale = materialSystem->createMaterial("GrayScalePostEffect");
    //    auto vignette = materialSystem->createMaterial("VignettePostEffect");
    //    auto pe1 = sceneGraph->makePosteffect("GrayScale", *grayscale);
    //    auto pe2 = sceneGraph->makePosteffect("Vignette", *vignette);
    //    camera.add<PosteffectCollection>({.posteffects = {pe1, pe2}});

    // setup window

    window->run();

  } catch (runtime_error& e) {
    spdlog::error(e.what());
    spdlog::error("Aborting the application with exit code ({})", EXIT_FAILURE);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

namespace {
// DEBUG_MODE is assumed tru as a default when built in Debug mode.
// In release mode, by default we don't want debug logging.
#ifndef NDEBUG
const char* DEBUG_MODE = "true";
#else
const char* DEBUG_MODE = "false";
#endif  // NDEBUG
}  // namespace

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]) {
  const std::string PROGRAM_NAME = "izzyrender";
  cxxopts::Options _options(PROGRAM_NAME, "Wera3d renderer.\n");
  _options.add_options()("d,debug", "Enable debug mode", cxxopts::value<bool>()->default_value(DEBUG_MODE))(
      "s,scene", "A scene file for visualization (*.fbx, *.obj)", cxxopts::value<std::string>())("m,materials", "Reference to a materials json file",
                                                                                                 cxxopts::value<std::string>())(
      "w,workspace", "Workspace directory", cxxopts::value<std::string>())("v,version", "Version information")("h,help", "Print usage");
  auto result = _options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << _options.help() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (result.count("version")) {
    std::cout << PROGRAM_NAME << " v" << VERSION_STR << std::endl;  // VERSION_STR is defined in CmakeLists.txt
    exit(EXIT_SUCCESS);
  }

  if (!result.count("scene")) {
    spdlog::error("Missing required command line argument --scene.");
    std::cout << _options.help() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->sceneFile = result["scene"].as<std::string>();
  workspace->materialsFile = result["materials"].as<std::string>();
  workspace->debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace->sceneFile << endl;
  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}