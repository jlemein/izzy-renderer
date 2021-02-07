// Copyright 2020 Jeffrey Lemein
#include <viewer.h>

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
#include <ecs_scenegraph.h>
#include <geo_shapeutil.h>
#include <glm/gtx/transform.hpp>

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

  Viewer viewer(sceneGraph, resourceManager);

  // Camera
  auto cameraTransform = glm::inverse(
      glm::lookAt(glm::vec3(0.0F, -10.60F, -1.0F), glm::vec3(0.0F, 0.0F, 0.0F),
                  glm::vec3(0.0F, 1.0F, 0.0F)));
  auto camera = sceneGraph.makeCamera(cameraTransform, "Camera");
  camera.add<FirstPersonControl>();

  // Grid
  sceneGraph.makeRectangularGrid();

//  auto sceneResource = resourceManager.getResource<geo::Scene>("assets/models/three_instanced_cubes_one_sphere.fbx");
//  sceneGraph.makeScene(**sceneResource);
  auto scene = sceneGraph.makeScene(resourceManager.getResource<geo::Scene>("testassets/models/3objects.fbx"));

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
  auto shader = Shader{"assets/shaders/diffuse-color.vert.spv",
                       "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock",
                     ColorBlock{glm::vec4{1.0F, 0.0F, 0.0F, 1.0F}});

  //  auto cylinder =
  //      EcsFactory(sceneGraph)
  //          .makeRenderable(geo::PrimitiveFactory::MakeCylinder(1.0F, 12.0F),
  //          shader);

  // add lighting to scene
  auto light = sceneGraph.makePointLight(
      glm::vec3{5.0F}, glm::vec3(1.0F, 1.0F, 0.7F), "PointLight");
  light.add<Debug>();
  TransformUtil::Translate(light.get<Transform>(),
                           glm::vec3(-2.0F, 3.0F, 0.0F));

  //  sceneGraph.emplace<ecs::Debug>(bunny);
  //  sceneGraph.emplace<FirstPersonControl>(bunny);
  //  auto curve = makeSierpinskiTriangle(sceneGraph);
  //  makePenroseTiling(sceneGraph);

  // Setup extended systems to enhance viewer functionality
  auto textureSystem = std::make_shared<TextureSystem>();

  viewer.registerExtension(textureSystem);
  viewer.registerRenderSubsystem(textureSystem);
  viewer.run();

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
