//
// Created by jlemein on 11-03-21.
//

#include "../lib/lsw-gl/include/izzgl_exrloader.h"
#include "../lib/lsw-gl/include/izzgl_sceneloader.h"
#include "../lib/lsw-gl/include/izzgl_stbtextureloader.h"
#include "../lib/lsw-gl/include/izzgl_texturesystem.h"
#include "anim_localrotation.h"
#include "core_util.h"
#include "ecs_firstpersoncontrol.h"
#include "ecs_light.h"
#include "ecs_transformutil.h"
#include "geo_meshutil.h"
#include "geo_scene.h"
#include "gui_iguiwindow.h"
#include "gui_lighteditor.h"
#include "gui_window.h"
#include "izz_fontsystem.h"
#include "izz_resourcemanager.h"
#include "izz_scenegraphhelper.h"
#include "izzgl_materialsystem.h"
#include "wsp_workspace.h"

#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <memory>
#include "geo_primitivefactory.h"
#include "ecs_camera.h"
using namespace std;
using namespace lsw;
using namespace izz;
using namespace lsw::geo;
using namespace glm;
using lsw::core::Util;
using lsw::wsp::Workspace;

std::shared_ptr<Workspace> parseProgramArguments(int argc, char* argv[]);

namespace {
std::shared_ptr<Workspace> programArguments{nullptr};
std::shared_ptr<ResourceManager> resourceManager{nullptr};
std::shared_ptr<MaterialSystem> materialSystem{nullptr};
std::shared_ptr<izz::SceneGraphHelper> sceneGraph{nullptr};
std::shared_ptr<glrs::RenderSystem> renderSystem{nullptr};
std::shared_ptr<SceneLoader> sceneLoader{nullptr};
std::shared_ptr<FontSystem> fontSystem {nullptr};
std::shared_ptr<gui::GuiSystem> guiSystem {nullptr};
}  // namespace

void setupSystems() {
  resourceManager = make_shared<ResourceManager>();
  sceneGraph = make_shared<izz::SceneGraphHelper>();

  auto textureSystem = make_shared<TextureSystem>();
  textureSystem->setTextureLoader(".exr", std::make_unique<ExrLoader>(true));
  textureSystem->setTextureLoader(ExtensionList{".jpg", ".png", ".bmp"}, std::make_unique<StbTextureLoader>(true));
  resourceManager->setTextureSystem(textureSystem);

  materialSystem = make_shared<MaterialSystem>(sceneGraph, resourceManager);
  resourceManager->setMaterialSystem(materialSystem);

  sceneLoader = make_shared<SceneLoader>(textureSystem, materialSystem);
  resourceManager->setSceneLoader(sceneLoader);

  renderSystem = make_shared<glrs::RenderSystem>(sceneGraph, materialSystem);
  fontSystem = make_shared<FontSystem>();
  guiSystem = make_shared<gui::GuiSystem>();
}

void setupLights() {
  // Sun
  sceneGraph->makeDirectionalLight("Sun", glm::vec3(0.F, 1.0F, 1.0F));

  // Point light 1
  auto ptLight1 = sceneGraph->makePointLight("PointLight 1", glm::vec3(1.F, 1.0F, -1.0F));
  auto& lightComp = ptLight1.get<ecs::PointLight>();
  lightComp.intensity = 1.0;
  lightComp.color = glm::vec3(1.0, 1.0, 1.0);
  ptLight1.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));

  // Point light 2
  auto ptLight2 = sceneGraph->makePointLight("PointLight 2", glm::vec3(-10.F, 1.0F, -1.0F));
  ptLight2.get<ecs::PointLight>().intensity = 1.4;
  ptLight2.add(PrimitiveFactory::MakeUVSphere("SphericalPointLight", 0.1));
}

void setupScene() {
  auto scene = sceneLoader->loadScene(programArguments->sceneFile);

  // post process meshes in scene file
  for (auto& mesh : scene->m_meshes) {
    MeshUtil::GenerateTangentsAndBitangentsFromUvCoords(*mesh);
    MeshUtil::GenerateSmoothNormals(*mesh);
  }

  // add to scene graph
  sceneGraph->makeScene(*scene, izz::SceneLoaderFlags::All());

  // adding a custom primitive to the scene
  //    auto plane = PrimitiveFactory::MakePlane("Plane", 25.0, 25.0);
  //    MeshUtil::ScaleUvCoords(plane, 3, 3);
  //    auto tableCloth = materialSystem->createMaterial("table_cloth");
  //    sceneGraph->addGeometry(plane, tableCloth);

}

void setupUserInterface() {
  guiSystem->addDialog(make_shared<gui::LightEditor>(sceneGraph, fontSystem));
}

int main(int argc, char* argv[]) {
  using namespace lsw::ecs;

  try {
    programArguments = parseProgramArguments(argc, argv);

    if (programArguments->debugMode) {
      spdlog::set_level(spdlog::level::debug);
    }

    setupSystems();

    if (programArguments->materialsFile.empty()) {
      spdlog::warn("No materials provided. Rendering results may be different than expected.");
    } else {
      materialSystem->loadMaterialsFromFile(programArguments->materialsFile);
    }

    // visualize point lights using a custom material.
    renderSystem->getLightSystem().setDefaultPointLightMaterial(materialSystem->createMaterial("pointlight"));

    setupScene();
    setupLights();
    setupUserInterface();

    // setup camera
    auto camera = sceneGraph->makeCamera("DummyCamera", 4);
    camera.add<ecs::FirstPersonControl>().onlyRotateOnMousePress = true;

    // configure bloom shader
    auto fbright = materialSystem->createMaterial("FilterBrightPostEffect");

    auto gaussianBlurH = materialSystem->createMaterial("GaussianBlur");
    auto gaussianBlurV = materialSystem->createMaterial("GaussianBlur");

    gaussianBlurH->unscopedUniforms.setBoolean("horizontal", true);
    gaussianBlurV->unscopedUniforms.setBoolean("horizontal", false);

    auto peFilterBright = sceneGraph->makePosteffect("FilterBright", *fbright);
    auto peGaussianBlur1 = sceneGraph->makePosteffect("GuassianBlur1", *gaussianBlurH);
    auto peGaussianBlur2 = sceneGraph->makePosteffect("GuassianBlur2", *gaussianBlurV);

    auto combine = materialSystem->createMaterial("CombineTextures");
    auto pe = sceneGraph->makePosteffect("Combine", *combine);

    camera.add<PosteffectCollection>({.posteffects = {peFilterBright, peGaussianBlur1, peGaussianBlur2, pe}});

    // setup window
    auto window = make_shared<gui::Window>(sceneGraph, renderSystem, guiSystem);  // guiSystem);
    window->setActiveCamera(camera);
    window->setWindowSize(1024, 768);
    window->setTitle(fmt::format("Izzy Renderer: {}", programArguments->sceneFile.filename().string()));
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