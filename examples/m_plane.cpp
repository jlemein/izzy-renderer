//
// Created by jlemein on 11-03-21.
//

#include "anim_localrotation.h"
#include "ecs_firstpersoncontrol.h"
#include "ecs_light.h"
#include "ecs_transformutil.h"
#include "geo_meshutil.h"
#include "geo_scene.h"
#include "izz_fontsystem.h"
#include "izzgl_entityfactory.h"

#include <izzgl_materialsystem.h>

#include "izzgui_window.h"
#include "wsp_workspace.h"

#include <ecs_transform.h>
#include <geo_mesh.h>
#include <izz_izzy.h>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
#include "geo_primitivefactory.h"
#include "izzgl_materialreader.h"
#include "izzgui_mainmenu.h"
#include "izzgui_stats.h"
using namespace std;
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
  izzy->entityFactory->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));

  // Point light 1
  auto ptLight1 = izzy->entityFactory->makePointLight("PointLight 1", glm::vec3(1.F, 1.0F, -1.0F));
  auto& lightComp = ptLight1.get<ecs::PointLight>();
  lightComp.intensity = 1.0;
  lightComp.color = glm::vec3(1.0, 1.0, 1.0);
  ptLight1.add<Debug>();
//  ptLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

  // Point light 2
  auto ptLight2 = izzy->entityFactory->makePointLight("PointLight 2", glm::vec3(-10.F, 1.0F, -1.0F));
  ptLight2.get<ecs::PointLight>().intensity = 1.4;
//  ptLight2.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));
}

void setupScene() {
  {
    auto box = izzy->entityFactory->makeMovableEntity("PlaneEntity");
    auto& mesh = box.add<Mesh>(PrimitiveFactory::MakePlane("Plane", 10.0, 10.0));
    auto& meshBuffer = izzy->meshSystem->createVertexBuffer(mesh);
    izz::ecs::TransformUtil::Translate(box.get<izz::ecs::Transform>(), glm::vec3(4.2, 0.0, 0.0));
    auto& material = izzy->materialSystem->createMaterial(programArguments->material);
    mesh.materialId = material.id;
    box.add<gl::DeferredRenderable>({material.id, meshBuffer.id});
  }

  // adding a custom primitive to the scene
//  auto plane = PrimitiveFactory::MakePlane("Plane", 25.0, 25.0);
//  MeshUtil::ScaleUvCoords(plane, 3, 3);
//  auto tableCloth = izzy->materialSystem->createMaterial("table_cloth");
////  auto effect = izzy->effectSystem->createEffect("table_cloth");
////  auto material = materialSystem->createMaterial("table_cloth");
//
//  izzy->entityFactory->addGeometry(plane, tableCloth.id);
//  e.add<gl::DeferredRenderable>();

  //    sceneGraph->addGeometry(plane, tableCloth);

}

void setupUserInterface() {
  izzy->guiSystem->addDialog(make_shared<gui::LightEditor>(izzy->entityFactory, izzy->fontSystem));
  izzy->guiSystem->addDialog(make_shared<gui::MainMenu>(*izzy));
  izzy->guiSystem->addDialog(make_shared<izz::gui::StatsDialog>());
}

int main(int argc, char* argv[]) {
  using namespace izz::ecs;

  try {
    programArguments = parseProgramArguments(argc, argv);

    if (programArguments->debugMode) {
      spdlog::set_level(spdlog::level::debug);
    }

    izzy = Izzy::CreateSystems();

    // window should be initialized before calling systems' functions
    auto window = make_shared<gui::Window>(*izzy);
    window->setWindowSize(1920, 1080);
    window->setTitle(fmt::format("Izzy Renderer: {}", programArguments->sceneFile.filename().string()));
    window->initializeContext();

    // onBeginFrame camera
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

    window->initialize();
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
      "t,material", "A material listed in the material file", cxxopts::value<std::string>())("m,materials", "Reference to a materials json file",
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

  if (!result.count("material")) {
    spdlog::error("Missing required material line argument --material.");
    std::cout << _options.help() << std::endl;
    exit(EXIT_FAILURE);
  }

  auto workspace = wsp::WorkspaceManager::GetActiveWorkspace();
  workspace->material = result["material"].as<std::string>();
  workspace->materialsFile = result["materials"].as<std::string>();
  workspace->debugMode = result["debug"].as<bool>();

  cout << "Scene file: " << workspace->sceneFile << endl;
  cout << "Materials file: " << (workspace->materialsFile.empty() ? "<not specified>" : workspace->materialsFile) << endl;
  cout << "Render using material: " << (workspace->material.empty() ? "<not specified>" : workspace->material) << endl;
  cout << "Strict mode: " << (workspace->isStrictMode ? "enabled" : "disabled") << endl;

  return workspace;
}