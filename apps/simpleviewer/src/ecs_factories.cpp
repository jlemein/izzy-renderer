//
// Created by jlemein on 12-11-20.
//
#include <ecs_factories.h>

#include <ecs_renderable.h>
#include <ecs_texture.h>
#include <ecs_relationship.h>
#include <ecs_transform.h>
#include <ecs_debug.h>
#include <ecs_name.h>
#include <geo_curve.h>
#include <geo_mesh.h>
#include <geo_meshloader.h>
#include <geo_meshtransform.h>
#include <geo_sceneloader.h>

#include <entt/entt.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

using namespace affx::ecs;

EcsFactory::EcsFactory(entt::registry &registry) : m_registry(registry) {}

entt::entity EcsFactory::makeEntity(std::string name) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Name>(e, name);
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<ecs::Relationship>(e);
  return e;
}

entt::entity EcsFactory::makeMesh(std::string sceneFile) {
  auto entity = m_registry.create();
  auto &transform = m_registry.emplace<ecs::Transform>(entity);
  auto &renderable = m_registry.emplace<ecs::Renderable>(entity);
  auto &mesh = m_registry.emplace<geo::Mesh>(entity);
  m_registry.emplace<ecs::Shader>(
      entity, ecs::Shader{"assets/shaders/diffuse.vert.spv",
                          "assets/shaders/diffuse.frag.spv"});

  geo::MeshLoader::loadSceneFile(sceneFile, mesh);

  transform.worldTransform =
      glm::translate(glm::mat4(1.0F), glm::vec3(1.0F, 0.0F, 1.0F));

  Texture &texture = m_registry.emplace<Texture>(entity);
  texture.diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg";

  renderable.name = sceneFile;

  return entity;
}

entt::entity EcsFactory::makeCurve() {
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

  return e;
}



entt::entity EcsFactory::makeScene(geo::Scene& scene) {
  auto root = makeEntity();

//  for(const auto& m : scene.nodeHierarchy()) {
//
//  }
  return entt::null;
}

entt::entity EcsFactory::makeRenderable(geo::Mesh&& mesh, const ecs::Shader& shader) {
    auto e = m_registry.create();
    m_registry.emplace<ecs::Transform>(e);
    m_registry.emplace<ecs::Renderable>(e);
    m_registry.emplace<geo::Mesh>(e, std::move(mesh));
    m_registry.emplace<ecs::Shader>(e, shader);

    return e;
}

entt::entity EcsFactory::makeRenderable(geo::Curve&& curve, const ecs::Shader& shader) {
  auto e = m_registry.create();
  m_registry.emplace<ecs::Transform>(e);
  m_registry.emplace<ecs::Renderable>(e);
  m_registry.emplace<geo::Curve>(e, std::move(curve));
  m_registry.emplace<ecs::Shader>(e, shader);

  return e;
}

entt::entity makeTexture();

entt::entity EcsFactory::makeRectangularGrid(float size, float spacing) {
  auto e = makeCurve();
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

  return e;
}
