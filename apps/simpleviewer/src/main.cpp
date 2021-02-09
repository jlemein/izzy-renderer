// Copyright 2020 Jeffrey Lemein
#include <vwr_viewer.h>

#include <ecs_camera.h>
#include <ecs_debug.h>
#include <ecs_factories.h>
#include <ecs_firstpersoncontrol.h>
#include <ecs_renderable.h>
#include <ecs_texturesystem.h>
#include <ecs_transform.h>
#include <ecs_transformutil.h>
#include <fractal_generator.h>
#include <fractal_penrosetiling.h>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_meshloader.h>
#include <geo_meshtransform.h>
#include <geo_primitivefactory.h>
#include <geo_sceneloader.h>
//#include <geo_scene.h>

#include <res_resource.h>
#include <res_resourcemanager.h>

#include <memory>

#include <ecs_light.h>
#include <ecs_name.h>
#include <ecsg_scenegraph.h>
#include <geo_shapeutil.h>
#include <glm/gtx/transform.hpp>
#include <gui_system.h>

namespace {
affx::res::ResourceManager resourceManager;
affx::ecs::SceneGraph sceneGraph;
} // namespace

// entt::entity makeBunny(entt::registry &registry, );
entt::entity makeLight(entt::registry &registry, const glm::vec3 &color,
                       float intensity);
entt::entity makeSierpinskiTriangle(entt::registry &registry);
entt::entity makePenroseTiling(entt::registry &registry);

using namespace affx;
using namespace affx::ecs;
using namespace affx::viewer;

int main() {
  resourceManager.addFactory<geo::Scene>(std::make_unique<geo::SceneLoader>());

  auto viewer = std::make_shared<Viewer>(sceneGraph, resourceManager);
  viewer->setWindowSize(1024, 1024);
  auto gui = std::make_shared<GuiSystem>(viewer);
  auto textureSystem = std::make_shared<TextureSystem>(viewer);
  viewer->registerExtension(gui);
  viewer->registerExtension(textureSystem);
  viewer->registerRenderSubsystem(textureSystem);

//  viewer.getInputHandler().addCommand(SWITCH_CAMERA, [](SceneGraph& scenegraph) {
//    auto cameras = scenegraph.getCameras();
//    auto camera = scenegraph.getActiveCamera();
//    scenegraph.setActiveCamera(nextCamera);
//  });
//  viewer.getEventHandler().addHandler(EVT_ENEMY_DOWN, [](SceneGraph& scenegraph) {
//    auto cameras = scenegraph.getCameras();
//    auto camera = scenegraph.getActiveCamera();
//    scenegraph.setActiveCamera(nextCamera);
//  });

  // Grid
  sceneGraph.makeRectangularGrid();

//  auto sceneResource = resourceManager.getResource<geo::Scene>("assets/models/three_instanced_cubes_one_sphere.fbx");
//  sceneGraph.makeScene(**sceneResource);

  auto loadedScene = resourceManager.getResource<geo::Scene>("testassets/models/3objects.fbx");
  auto sg = sceneGraph.makeScene(loadedScene);

  // Camera
  auto cameraTransform = glm::inverse(
      glm::lookAt(glm::vec3(0.0F, 1.60F, 10.0F), glm::vec3(0.0F, 1.60F, 0.0F),
                  glm::vec3(0.0F, 1.0F, 0.0F)));
  auto camera = sceneGraph.makeCamera("CodeCamera");
  camera.setTransform(cameraTransform);
  camera.add<FirstPersonControl>();
  viewer->setActiveCamera(camera);

  auto fakeCamera = sceneGraph.makeCamera("FakeCamera");
  fakeCamera.setTransform(cameraTransform);
  fakeCamera.add<Debug>({.shape = ecs::DebugShape::kCamera});

  // Bunny
  auto bunnyScene =
      resourceManager.getResource<geo::Scene>("assets/models/bunny.fbx");
  auto bunnyMesh = bunnyScene->meshes()[0];
  auto bunny = sceneGraph.makeMesh(*bunnyMesh);
  geo::MeshTransform::ScaleToUniformSize(bunny.get<geo::Mesh>());
  TransformUtil::Translate(bunny.get<Transform>(), glm::vec3(4.0F, 1.0F, 1.0F));
  bunny.add<Debug>(Debug{.shape = DebugShape::kEulerArrow});

  // ShaderResources resources;
  // resources.init(); // loads shader files
  // ShaderResources.getNamedShader("diffuse-color");
//  auto shader = Shader{"assets/shaders/diffuse-color.vert.spv",
//                       "assets/shaders/diffuse-color.frag.spv"};
//  shader.setProperty("ColorBlock",
//                     ColorBlock{glm::vec4{1.0F, 0.0F, 0.0F, 1.0F}});

  //  auto cylinder =
  //      EcsFactory(sceneGraph)
  //          .makeRenderable(geo::PrimitiveFactory::MakeCylinder(1.0F, 12.0F),
  //          shader);

  // add lighting to scene
  auto light = sceneGraph.makePointLight("MyPointLight", glm::vec3(1.0F, 1.0F, 0.7F));
  light.get<Light>().intensity = 1000.0F;
  light.add<Debug>();
  TransformUtil::Translate(light.get<Transform>(),
                           glm::vec3(-2.0F, 3.0F, 0.0F));

  //  sceneGraph.emplace<ecs::Debug>(bunny);
  //  sceneGraph.emplace<FirstPersonControl>(bunny);
  //  auto curve = makeSierpinskiTriangle(sceneGraph);
  //  makePenroseTiling(sceneGraph);

  viewer->initialize();
  viewer->run();

  return 0;
}

entt::entity makePenroseTiling(entt::registry &registry) {
  auto e = registry.create();
  auto &rr = registry.emplace<Renderable>(e);
  registry.emplace<Name>(e, "Penrose");

  auto &shader = registry.emplace<Shader>(
      e, Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
                .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});
  shader.setProperty("ColorBlock",
                     ColorBlock{glm::vec4(1.0F, 0.5F, 0.0F, 0.0F)});

  PenroseTiling tiling(10.0F);
  tiling.decompose();
  tiling.decompose();
  auto &curve = registry.emplace<geo::Curve>(e);
  tiling.fillVertices(curve.vertices);

  return e;
}

entt::entity makeSierpinskiTriangle(entt::registry &registry) {
  auto e = registry.create();

  auto &rr = registry.emplace<Renderable>(e);
  registry.emplace<Name>(e, "FractalTree");

  geo::Curve &fractalCurve = registry.emplace<geo::Curve>(
      e, affx::fractal::FractalGenerator(0).makeSierpinskiTriangle(8));
  geo::MeshTransform::ScaleToUniformSize(fractalCurve);
  auto &shader = registry.emplace<Shader>(
      e, Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
                .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});
  shader.setProperty("ColorBlock",
                     ColorBlock{glm::vec4(1.0F, 1.0F, 1.0F, 0.0F)});

  registry.emplace<ecs::Transform>(
      e, ecs::Transform{.worldTransform = glm::rotate(
                            glm::radians(0.0F), glm::vec3(0.0F, 0.0F, 1.0F))});
  return e;
}
