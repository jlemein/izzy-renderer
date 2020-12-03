//
// Created by jlemein on 12-11-20.
//

#include <ecs_renderable.h>
#include <ecs_texture.h>
#include <ecs_transform.h>
#include <shp_mesh.h>
#include <shp_curve.h>
#include <shp_meshloader.h>
#include <shp_meshtransform.h>

#include <entt/entt.hpp>
#include <glm/gtx/transform.hpp>

#ifndef GLVIEWER_ECS_FACTORIES_H
#define GLVIEWER_ECS_FACTORIES_H

namespace artifax
{
class EcsFactory
{
  entt::registry &m_registry;

 public:
  EcsFactory(entt::registry &registry) : m_registry(registry) {}

  entt::entity
  makeMesh(std::string sceneFile)
  {
    auto entity = m_registry.create();
    auto &transform = m_registry.emplace<ecs::Transform>(entity);
    auto &renderable = m_registry.emplace<ecs::Renderable>(entity);
    auto &mesh = m_registry.emplace<shp::Mesh>(entity);
    m_registry.emplace<ecs::Shader>(
      entity,
      ecs::Shader{"assets/shaders/diffuse.vert.spv", "assets/shaders/diffuse.frag.spv"});

    shp::MeshLoader::loadSceneFile(sceneFile, m_registry.get<shp::Mesh>(entity));

    transform.worldTransform = glm::translate(glm::mat4(1.0F),
                                              glm::vec3(1.0F, 0.0F, 1.0F));

    Texture &texture = m_registry.emplace<Texture>(entity);
    texture.diffuseTextureFilepath = "assets/textures/textures/castle_normal.jpg";

    renderable.name = sceneFile;

    return entity;
  }

  entt::entity
  makeCurve()
  {
    auto e = m_registry.create();
    auto &curve = m_registry.emplace<shp::Curve>(e);
    auto &r = m_registry.emplace<ecs::Renderable>(e);
    auto &t = m_registry.emplace<ecs::Transform>(e);
    auto &s = m_registry.emplace<ecs::Shader>(
      e, ecs::Shader{.vertexShaderFile = "assets/shaders/default_curve.vert.spv",
        .fragmentShaderFile = "assets/shaders/default_curve.frag.spv"});

    ecs::ColorBlock block;
    block.color = glm::vec4(1.0F, 0.35F, 1.0F, 0.0F);
    s.setProperty("ColorBlock", block);

    return e;
  }

  entt::entity makeTexture();

  entt::entity
  makeRectangularGrid(float size = 10.0F, float spacing = 1.0F)
  {
    auto e = makeCurve();
    auto &curve = m_registry.get<shp::Curve>(e);
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
};
}  // namespace artifax
#endif  // GLVIEWER_ECS_FACTORIES_H
