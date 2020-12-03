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

#include <glm/gtx/transform.hpp>
#include <memory>


entt::entity makeBunny(entt::registry &registry);
entt::entity makeSierpinskiTriangle(entt::registry &registry);
entt::entity makePenroseTiling(entt::registry &registry);

using namespace artifax;
using namespace artifax::ecs;
using namespace artifax::shp;
using namespace artifax::viewer;

int
main()
{
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
//  sceneGraph.emplace<ecs::Debug>(bunny);

  //  sceneGraph.emplace<FirstPersonControl>(bunny);

  //  auto curve = makeSierpinskiTriangle(sceneGraph);
  auto penrose = makePenroseTiling(sceneGraph);

  auto textureSystem = std::make_shared<TextureSystem>();

  viewer.registerExtension(textureSystem);
  viewer.registerRenderSubsystem(textureSystem);
  viewer.run();

  return 0;
}

entt::entity
makePenroseTiling(entt::registry &registry)
{
  auto e = registry.create();
  auto &rr = registry.emplace<Renderable>(e);
  auto &shader = registry.emplace<Shader>(
    e, Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
              .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});
  shader.setProperty("ColorBlock", ColorBlock{glm::vec4(1.0F, 0.5F, 0.0F, 0.0F)});

  PenroseTiling tiling(10.0F);
  tiling.decompose();
  tiling.decompose();
  auto &curve = registry.emplace<shp::Curve>(e);
  tiling.fillVertices(curve.vertices);

  return e;
}

entt::entity
makeSierpinskiTriangle(entt::registry &registry)
{
  auto e = registry.create();

  auto &rr = registry.emplace<Renderable>(e);
  rr.name = "FractalTree";

  Curve &fractalCurve = registry.emplace<Curve>(
    e, artifax::fractal::FractalGenerator(0).makeSierpinskiTriangle(8));
  shp::MeshTransform::scaleToUniformSize(fractalCurve);
  auto &shader = registry.emplace<Shader>(
    e, Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
              .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});
  shader.setProperty("ColorBlock", ColorBlock{glm::vec4(1.0F, 1.0F, 1.0F, 0.0F)});

  registry.emplace<ecs::Transform>(
    e, ecs::Transform{
         .worldTransform = glm::rotate(glm::radians(0.0F), glm::vec3(0.0F, 0.0F, 1.0F))});
  return e;
}

entt::entity
makeBunny(entt::registry &registry)
{
  auto e = EcsFactory(registry).makeMesh("./assets/models/bunny.fbx");
  shp::MeshTransform::scaleToUniformSize(registry.get<Mesh>(e));

  auto rotate = glm::rotate(glm::radians(-180.0F), glm::vec3(0.0F, 1.0F, 0.0F));
  auto translate = glm::translate(glm::mat4(1.0F), glm::vec3(0.0F, 0.0F, 2.0F));
  registry.get<ecs::Transform>(e).worldTransform = translate * rotate;
  return e;
}
