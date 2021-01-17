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
#include <shp_curve.h>
#include <shp_mesh.h>
#include <shp_meshloader.h>
#include <shp_meshtransform.h>
#include <shp_primitivefactory.h>

#include <memory>

#include <ecs_light.h>
#include <ecs_name.h>
#include <glm/gtx/transform.hpp>
#include <shp_shapeutil.h>

entt::entity makeBunny(entt::registry &registry);
entt::entity makeLight(entt::registry &registry, const glm::vec3& color, float intensity);
entt::entity makeSierpinskiTriangle(entt::registry &registry);
entt::entity makePenroseTiling(entt::registry &registry);

using namespace artifax;
using namespace artifax::ecs;
using namespace artifax::shp;
using namespace artifax::viewer;

#include <res_resourcemanager.h>

int main() {
  afxx::res::ResourceManager resourceManager;
  resourceManager.getResource<shp::Mesh>("mesh.fbx");


  Viewer viewer;

  Viewer::SceneGraph &sceneGraph = viewer.getRegistry();

  auto e = sceneGraph.create();
  sceneGraph.emplace<Camera>(e);
  auto &t = sceneGraph.emplace<Transform>(e);
  //  TransformUtil::SetPosition(t, {0.0F, 1.60, -1.0F});
  t.localTransform = glm::inverse(glm::lookAt(glm::vec3(0.0F, -10.60F, -1.0F),
                                              glm::vec3(0.0F, 0.0F, 0.0F),
                                              glm::vec3(0.0F, 1.0F, 0.0F)));

  //  sceneGraph.emplace<Orientation>(e);
  //  sceneGraph.emplace<Position>(e);
  sceneGraph.emplace<FirstPersonControl>(e);

  EcsFactory(sceneGraph).makeRectangularGrid();
  auto bunny = makeBunny(sceneGraph);
  auto &transform = sceneGraph.get<Transform>(bunny);
  TransformUtil::Translate(transform, glm::vec3(4.0F, 1.0F, 1.0F));
  sceneGraph.emplace<Debug>(bunny, Debug{.shape = DebugShape::kEulerArrow});

  // ShaderResources resources;
  // resources.init(); // loads shader files
  // ShaderResources.getNamedShader("diffuse-color");
  auto shader = Shader{"assets/shaders/diffuse-color.vert.spv",
                       "assets/shaders/diffuse-color.frag.spv"};
  shader.setProperty("ColorBlock", ColorBlock{glm::vec4{1.0F, 0.0F, 0.0F, 1.0F}});

//  auto cylinder =
//      EcsFactory(sceneGraph)
//          .makeRenderable(shp::PrimitiveFactory::MakeCylinder(1.0F, 12.0F), shader);

  // add lighting to scene
  auto light = makeLight(sceneGraph, glm::vec3(1.0F, 1.0F, 0.7F), 5.0F);
  sceneGraph.emplace<Name>(light, "PointLight");
  sceneGraph.emplace<Debug>(light);
  auto& lightTransform = sceneGraph.get<Transform>(light);
  TransformUtil::Translate(lightTransform, glm::vec3(-2.0F, 3.0F, 0.0F));

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

entt::entity makeLight(entt::registry &registry, const glm::vec3& color, float intensity) {
  auto e = registry.create();
  registry.emplace<Transform>(e);
  registry.emplace<ecs::Light>(e, ecs::Light{glm::vec3{intensity}, color, false});
  return e;
}

entt::entity makePenroseTiling(entt::registry &registry) {
  auto e = registry.create();
  auto &rr = registry.emplace<Renderable>(e);
  rr.name = "Penrose";
  auto &shader = registry.emplace<Shader>(
      e, Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
                .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});
  shader.setProperty("ColorBlock",
                     ColorBlock{glm::vec4(1.0F, 0.5F, 0.0F, 0.0F)});

  PenroseTiling tiling(10.0F);
  tiling.decompose();
  tiling.decompose();
  auto &curve = registry.emplace<shp::Curve>(e);
  tiling.fillVertices(curve.vertices);

  return e;
}

entt::entity makeSierpinskiTriangle(entt::registry &registry) {
  auto e = registry.create();

  auto &rr = registry.emplace<Renderable>(e);
  rr.name = "FractalTree";

  Curve &fractalCurve = registry.emplace<Curve>(
      e, artifax::fractal::FractalGenerator(0).makeSierpinskiTriangle(8));
  shp::MeshTransform::scaleToUniformSize(fractalCurve);
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

entt::entity makeBunny(entt::registry &registry) {
  auto e = EcsFactory(registry).makeMesh("./assets/models/bunny2.fbx");
  shp::MeshTransform::scaleToUniformSize(registry.get<Mesh>(e));

  // auto rotate = glm::rotate(glm::radians(-180.0F), glm::vec3(0.0F, 1.0F,
  // 0.0F)); auto translate = glm::translate(glm::mat4(1.0F), glm::vec3(0.0F,
  // 0.0F, 0.0F));
  //  registry.get<ecs::Transform>(e).localTransform = translate * rotate;
  return e;
}
