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
#include "ecs_firstpersoncontrol.h"
#include "ecs_light.h"
#include "ecs_transformutil.h"
#include "geo_meshutil.h"
#include "geo_scene.h"
#include "gui_iguiwindow.h"
#include "gui_lighteditor.h"
#include "izz_entityfactory.h"
#include "izz_fontsystem.h"
#include "izz_resourcemanager.h"
#include "izzgl_exrloader.h"
#include "izzgl_sceneloader.h"
#include "izzgl_stbtextureloader.h"
#include "izzgl_texturesystem.h"

#include "gui_window.h"
#include "wsp_workspace.h"

#include <ecs_transformutil.h>
#include <izz_izzy.h>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
#include "ecs_camera.h"
#include "geo_primitivefactory.h"
#include "gui_mainmenu.h"
#include <izzgui_stats.h>
using namespace std;
using namespace izz;
using namespace izz;
using namespace izz::geo;
using namespace glm;
using izz::wsp::Workspace;

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]);

namespace {
std::shared_ptr<izz::Izzy> izzy {nullptr};
std::shared_ptr<Workspace> programArguments{nullptr};
}  // namespace

void setupLights() {
  // Sun
  auto sun = izzy->entityFactory->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));
  sun.get<ecs::DirectionalLight>().intensity = 0.1F;

  // Ambient light
  auto ambientLight1 = izzy->entityFactory->makeAmbientLight("Ambient", glm::vec3(0.1F, 0.0F, 0.2F));
  ambientLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

//
  // Spot light 1
  auto spotLight1 = izzy->entityFactory->makeSpotLightFromLookAt("SpotLight", glm::vec3(0.1F, 2.0F, 0.0F));
  auto& spot = spotLight1.get<ecs::SpotLight>();
  spot.intensity = 1.0;
  spot.color = glm::vec3(1.0, 0.3, .3);
  spotLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

  // Point light 1
  auto ptLight1 = izzy->entityFactory->makePointLight("PointLight 1", glm::vec3(1.F, 1.0F, -1.0F));
  auto& lightComp = ptLight1.get<ecs::PointLight>();
  lightComp.intensity = 1.0;
  lightComp.color = glm::vec3(1.0, 1.0, 1.0);
  ptLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

  // Point light 2
  auto ptLight2 = izzy->entityFactory->makePointLight("PointLight 2", glm::vec3(-10.F, 1.0F, -1.0F));
  ptLight2.get<ecs::PointLight>().intensity = 1.4;
  ptLight2.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));
}

void setupScene() {
  auto scene = izzy->sceneLoader->loadScene(programArguments->sceneFile);

  // post process meshes in scene file
  for (auto& mesh : scene->m_meshes) {
    MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(*mesh);
    MeshUtil::GenerateSmoothNormals(*mesh);
  }

  // add to scene graph
  izzy->entityFactory->setDefaultRenderStrategy(gl::RenderStrategy::DEFERRED);
  izzy->entityFactory->makeScene(*scene, izz::SceneLoaderFlags::All());

  {
    // adding a custom primitive to the scene
    auto plane = izzy->entityFactory->makeMoveableEntity("Plane");
    auto& mesh = plane.add<Mesh>(PrimitiveFactory::MakePlane("MyPlane", 15, 15));
    auto& meshBuffer = izzy->meshSystem->createMeshBuffer(mesh);
    auto& material = izzy->materialSystem->createMaterial("DeferredStandard_Color");
    mesh.materialId = material.id;
    auto& dr = plane.add<gl::DeferredRenderable>({material.id, meshBuffer.id});
  }
  {
    auto box = izzy->entityFactory->makeMoveableEntity("Box");
    auto& mesh = box.add<Mesh>(PrimitiveFactory::MakeBox("MyBox", .5, .5));
    auto& meshBuffer = izzy->meshSystem->createMeshBuffer(mesh);
    izz::ecs::TransformUtil::Translate(box.get<izz::ecs::Transform>(), glm::vec3(0.2, 0.0, 0.0));
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
  izzy->guiSystem->addDialog(make_shared<gui::LightEditor>(izzy->entityFactory, izzy->fontSystem));
  izzy->guiSystem->addDialog(make_shared<gui::MainMenu>(*izzy));
  izzy->guiSystem->addDialog(make_shared<gui::StatsDialog>());
}

int main(int argc, char* argv[]) {
  using namespace izz::ecs;

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
    auto camera = izzy->entityFactory->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;

    if (programArguments->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      izz::gl::MaterialReader reader(izzy->materialSystem);
      reader.readMaterials(programArguments->materialsFile);
    }

    setupScene();
    setupLights();

    // visualize point lights using a custom material.
    izzy->renderSystem->getLightSystem().setDefaultPointLightMaterial(izzy->materialSystem->createMaterial("pointlight").id);
    izzy->renderSystem->init(window->getDisplayDetails().windowWidth, window->getDisplayDetails().windowHeight);

    setupUserInterface();

    //    auto grayscale = materialSystem->createMaterial("GrayScalePostEffect");
    //    auto vignette = materialSystem->createMaterial("VignettePostEffect");
    //    auto pe1 = sceneGraph->makePosteffect("GrayScale", *grayscale);
    //    auto pe2 = sceneGraph->makePosteffect("Vignette", *vignette);
    //    camera.add<PosteffectCollection>({.posteffects = {pe1, pe2}});

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