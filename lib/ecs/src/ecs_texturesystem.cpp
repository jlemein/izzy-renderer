//
// Created by jlemein on 06-11-20.
//

#include <ecs_texturesystem.h>
#include <ecs_renderable.h>
#include <ecs_texture.h>

#include <GL/glew.h>
#include <stb_image.h>

#include <iostream>

using namespace affx::ecs;

namespace {
void setupTextures(Texture &texture) {

  glGenTextures(1, &texture.glTextureId);
  glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;

  unsigned char *data = stbi_load(texture.diffuseTextureFilepath.c_str(),
                                  &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    std::cout << "Failed to load texture" << std::endl;
    stbi_image_free(data);
    exit(1);
  }

  // done
}

}

void TextureSystem::init(entt::registry &registry) {
  auto view = registry.view<Texture>();

  for (auto entity : view) {
    if (!registry.has<Texture>(entity)) {
      std::cout << "Cannot find TextureComponent in Mesh\n";
      exit(1);
    }

    auto &texture = view.get<Texture>(entity);

    setupTextures(texture);
  }

  //  auto pTexture = reinterpret_cast<Texture*>(mesh.getComponent("Texture"));
}

void TextureSystem::update(entt::registry &registry, float time, float dt) {}

void TextureSystem::onRender(const entt::registry &registry,
                             entt::entity entity) {
  //  for (auto entity : view) {
  if (!registry.has<Renderable, Texture>(entity)) {
    return;
  }

  const auto &renderable = registry.get<Renderable>(entity);
  const auto &texture = registry.get<Texture>(entity);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

  GLint s_diffuseMap = glGetUniformLocation(renderable.program, "diffuseTexture");
  //    if (s_diffuseMap == -1) {
  //        //std::cout << "Cannot find diffuse texture" << std::endl;
  //        //exit(1);
  //    } else {
  glUniform1i(s_diffuseMap, 0);

  //    }
  //    glBindSampler(0, 0);
}