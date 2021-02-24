//
// Created by jlemein on 06-11-20.
//

#include <ecs_texturesystem.h>
#include <ecs_renderable.h>
#include <ecs_texture.h>
#include <ecsg_scenegraph.h>
#include <geo_material.h>
#include <geo_texture.h>

#include <GL/glew.h>

#include <iostream>
#include <spdlog/spdlog.h>

using namespace affx::ecs;
using namespace affx;

TextureSystem::TextureSystem(std::shared_ptr<ecsg::SceneGraph> sceneGraph)
: m_sceneGraph(sceneGraph)
{
}

//void TextureSystem::initialize() {
//  auto& registry = m_sceneGraph.getRegistry();
//  auto view = registry.view<Texture>();
//
//  for (auto entity : view) {
//    auto &texture = view.get<Texture>(entity);
//
//    setupTextures(texture);
//  }
//
//  //  auto pTexture = reinterpret_cast<Texture*>(mesh.getComponent("Texture"));
//}

void TextureSystem::initialize() {
  auto& registry = m_sceneGraph->getRegistry();
  auto view = registry.view<geo::Material>();

  for (auto entity : view) {
    auto &geoMaterial = view.get<geo::Material>(entity);
    auto& shader = registry.get_or_emplace<Shader>(entity);

    // diffuse texture
    if (geoMaterial.diffuseTexture != nullptr) {
      auto geoTexture_p = *geoMaterial.diffuseTexture;

      Texture diffuseTexture {.name = "diffuseTex" };
      glGenTextures(1, &diffuseTexture.glTextureId);
      glBindTexture(GL_TEXTURE_2D, diffuseTexture.glTextureId);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (!geoTexture_p->data.empty()) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, geoTexture_p->width, geoTexture_p->height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, geoTexture_p->data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
      } else {
        std::cout << "Failed to load texture" << std::endl;
        exit(1);
      }

      shader.textures.emplace_back(diffuseTexture);

      spdlog::log(spdlog::level::debug, "Added diffuse texture entity for material {}", geoMaterial.name);
    }
  }
}

void TextureSystem::update(float time, float dt) {}

void TextureSystem::onRender(entt::entity e) {
  //  for (auto entity : view) {
  auto& registry = m_sceneGraph->getRegistry();

  if (!registry.has<Renderable, Shader>(e)) {
    return;
  }

  const auto &renderable = registry.get<Renderable>(e);
  const auto& shader = registry.get<Shader>(e);

  for (int t=0; t<shader.textures.size(); ++t) {
    auto& texture = shader.textures[t];

    glActiveTexture(GL_TEXTURE0+t);
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

    GLint s_diffuseMap = glGetUniformLocation(renderable.program, texture.name.c_str());
    //    if (s_diffuseMap == -1) {
    //        //std::cout << "Cannot find diffuse texture" << std::endl;
    //        //exit(1);
    //    } else {
    glUniform1i(s_diffuseMap, t);
  }


  //    }
  //    glBindSampler(0, 0);
}