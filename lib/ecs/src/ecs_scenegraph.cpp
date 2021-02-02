#include <ecs_scenegraph.h>

#include <ecs_camera.h>
#include <ecs_light.h>
#include <ecs_name.h>
#include <ecs_relationship.h>
#include <ecs_renderable.h>
#include <ecs_texture.h>
#include <ecs_transform.h>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_meshinstance.h>
#include <geo_sceneloader.h>
#include <ecs_scenegraphentity.h>
#include <geo_meshtransform.h>
using namespace affx;
using namespace affx::ecs;

SceneGraphEntity SceneGraph::makeEntity(std::string name) {
  auto e { m_registry.create() };
  m_registry.emplace<Name>(e, name);
  m_registry.emplace<Transform>(e);
  m_registry.emplace<ecs::Relationship>(e);
  return SceneGraphEntity { m_registry, e };
}

SceneGraphEntity SceneGraph::makeCamera(const glm::mat4 &transform,
                                          std::string name) {
  auto sge = this->makeEntity(std::move(name));
  auto e = sge.id();
  m_registry.emplace<ecs::Camera>(e);
  auto &t = m_registry.get<Transform>(e);
  t.localTransform = transform;
  //  TransformUtil::SetPosition(t, {0.0F, 1.60, -1.0F});
  return sge;
}

SceneGraphEntity SceneGraph::makePointLight(glm::vec3 intensity, glm::vec3 color,
                                              std::string name) {
  auto sge = this->makeEntity(std::move(name));
  auto e = sge.id();
  m_registry.emplace<ecs::Light>(
      e, ecs::Light{std::move(intensity), std::move(color), false});
  return sge;
}

SceneGraphEntity SceneGraph::makeMesh(const geo::Mesh &mesh) {
  auto sge = makeEntity(mesh.name);
  auto e = sge.id();

  auto &renderable = m_registry.emplace<ecs::Renderable>(e);
  renderable.name = mesh.name;

  m_registry.emplace<geo::Mesh>(e, mesh);
  m_registry.emplace<ecs::Shader>(
      e, ecs::Shader{"assets/shaders/diffuse.vert.spv",
                          "assets/shaders/diffuse.frag.spv"});

//  m_registry.get<Transform>(e).worldTransform =
//      glm::translate(glm::mat4(1.0F), glm::vec3(1.0F, 0.0F, 1.0F));

  Texture &texture = m_registry.emplace<Texture>(e);
  texture.diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg";

  return sge;
}

SceneGraphEntity SceneGraph::makeCurve() {
  auto e = m_registry.create();

  auto &curve = m_registry.emplace<geo::Curve>(e);
  auto &r = m_registry.emplace<ecs::Renderable>(e);
  auto &t = m_registry.emplace<ecs::Transform>(e);
  auto &s = m_registry.emplace<ecs::Shader>(
      e, ecs::Shader{
             .vertexShaderFile = "assets/shaders/default_curve.vert.spv",
             .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});

  ecs::ColorBlock block;
  block.color = glm::vec4(1.0F, 0.35F, 1.0F, 0.0F);
  s.setProperty("ColorBlock", block);

  return SceneGraphEntity { m_registry, e };
}

SceneGraphEntity SceneGraph::makeScene(geo::Scene &scene) {
  std::cout << "WARN: copying a scene, very tricky" << std::endl;
  auto rootScene = makeEntity();

  for (const auto& n : scene.nodeHierarchy()) {
    for (const auto& m : n.meshInstances) {
      auto instance = makeMesh(*m->mMesh);
      geo::MeshTransform::ScaleToUniformSize(instance.get<geo::Mesh>());
      instance.setTransform(n.transform);
      rootScene.addChild(instance);
    }
  }
//  for(const auto& m : scene.meshes()) {
//    rootScene.addChild(makeMesh(*m));
//  }

  return rootScene;
}

SceneGraphEntity SceneGraph::makeRenderable(geo::Mesh &&mesh,
                                              const ecs::Shader &shader) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<ecs::Renderable>(e);
  m_registry.emplace<geo::Mesh>(e, std::move(mesh));
  m_registry.emplace<ecs::Shader>(e, shader);

  return SceneGraphEntity { m_registry, e };
}

SceneGraphEntity SceneGraph::makeRenderable(geo::Curve &&curve,
                                              const ecs::Shader &shader) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<ecs::Renderable>(e);
  m_registry.emplace<geo::Curve>(e, std::move(curve));
  m_registry.emplace<ecs::Shader>(e, shader);

  return SceneGraphEntity { m_registry, e };
}

entt::entity makeTexture();

SceneGraphEntity SceneGraph::makeRectangularGrid(float size, float spacing) {
  auto sge = makeCurve();
  auto e = sge.id();
  auto &curve = m_registry.get<geo::Curve>(e);
  auto &r = m_registry.get<ecs::Renderable>(e);
  r.name = "RectangularGrid";

  float z = 0.0F;

  // initial center line axis
  curve.vertices.push_back(-size);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(z);

  curve.vertices.push_back(size);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(z);

  // initial center line axis
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(-size);

  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(0.0F);
  curve.vertices.push_back(size);

  while (z < size) {
    // positive side horizontal
    curve.vertices.push_back(-size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(z);

    curve.vertices.push_back(size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(z);

    curve.vertices.push_back(-size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-z);

    curve.vertices.push_back(size);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-z);

    // positive side vertical
    curve.vertices.push_back(z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-size);

    curve.vertices.push_back(z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(size);

    curve.vertices.push_back(-z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(-size);

    curve.vertices.push_back(-z);
    curve.vertices.push_back(0.0F);
    curve.vertices.push_back(size);

    z += spacing * 2.0F;
  }

  return sge;
}
